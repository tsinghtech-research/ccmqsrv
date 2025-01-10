#include <cstring>
#include <cstdlib>
#include <thread>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "common.h"
#include "server.h"

namespace ccmqsrv {
void MessageQueueServer::setup() {
  if (is_setuped)
    return;
  spdlog::info("server setting up...");
  is_setuped = true;
  spdlog::info("server setuped.");
}

void MessageQueueServer::teardown() {
  if (!is_setuped)
    return;
  spdlog::info("server tearing down...");
  is_setuped = false;
  spdlog::info("server teardown");
}

void MessageQueueServer::run() {
  ConnHandler c_handler;
  AMQP::Address address(conn_n);
  AMQP::TcpConnection conn(c_handler, address);

  recv_channel = std::make_shared<AMQP::TcpChannel>(&conn);
  send_channel = std::make_shared<AMQP::TcpChannel>(&conn);

  // flags durable(0x1) passive(0x8) autodelete(0x2) exclusive(0x200) internal(0x10000) nowait(0x400)
  // rpc exchange
  recv_channel->declareExchange(this->rpc_exchange_n, AMQP::topic, AMQP::durable);
  // event exchange
  recv_channel->declareExchange(this->event_exchange_n, AMQP::topic, AMQP::durable);
  
  // set qos
  // prefetchCount 1
  // global false
  recv_channel->setQos(1, false);
  if (!rpc_queue_n.empty()) {
    recv_channel->declareQueue(this->rpc_queue_n, AMQP::exclusive+AMQP::autodelete);
    recv_channel->bindQueue(this->rpc_exchange_n, this->rpc_queue_n, this->rpc_queue_n);
    recv_channel->consume(this->rpc_queue_n)
      .onReceived(
        [this](const AMQP::Message &message, uint64_t tag, bool) {
          auto req_id = message.correlationID();
          auto reply_to = message.replyTo();
          _on_rpc_message(message, string(message.body(), message.body()+message.bodySize()), req_id, reply_to);
          recv_channel->ack(tag);
        }
      );
  }
  if (!event_queue_n.empty()) {
    recv_channel->declareQueue(this->event_queue_n, AMQP::durable);
    recv_channel->bindQueue(this->event_exchange_n, this->event_queue_n, this->event_queue_n);
    recv_channel->consume(this->event_queue_n, AMQP::noack)
      .onReceived(
        [this](const AMQP::Message &message,  uint64_t, bool) {
          _on_event_message(message);
        }
      );
  }
  
  spdlog::debug("connected to rpc and event queues");
  c_handler.loop();
  conn.close();
}

void MessageQueueServer::_on_rpc_message(const AMQP::Message &message, const string &body, const string &req_id, const string &reply_to) {
  pool.detach_task([&message, body, req_id, reply_to, this]{_rpc_worker(message, body, req_id, reply_to);});
}

void MessageQueueServer::_on_event_message(const AMQP::Message &message) {
  string body = message.body();
  auto body_vec = json_loadb(body.substr(0, message.bodySize()));
  string event_type = std::any_cast<string>(body_vec[0]);
  std::any event_data = body_vec[1];
  
  if (event_handler_map.find(event_type) == event_handler_map.end()) {
    spdlog::error("event type {} not found", event_type);
    return;
  }
  
  for (auto &[cb_id, cb] : event_handler_map[event_type]) {
    pool.detach_task([cb_id, event_type, event_data, this]{_event_worker(event_type, cb_id, event_data);});
  }
}

void MessageQueueServer::_event_worker(const string &event_type, int cb_id, const std::any &event_data) {
  spdlog::debug("reciving event [{}])", event_type);
  try {
    Res res;
    std::vector<std::any> args{event_type, event_data};
    event_handler_map[event_type][cb_id](&res, args);
  } catch(const std::runtime_error &e) {
    spdlog::error("BaseException for event {}\n{}", event_type, e.what());
  } catch(const std::exception &e) {
    spdlog::error("BUG for event {}\n{}", event_type, e.what());
    throw e;
  }    
}

void MessageQueueServer::_rpc_worker(const AMQP::Message &message, const string &body, const string &req_id, const string &reply_to) {
  auto body_vec = json_loadb(body);
  string id{""};
  string meth{""};
  std::vector<std::any> args;
  std::map<string, std::any> kws;
  rpc_decode_req(body_vec, id, meth, args, kws, req_id);
  spdlog::debug("reciving request [{}, {}] {}", this->rpc_queue_n, req_id, meth);
  auto reply = std::bind(&MessageQueueServer::send_reply, this, &message, _1);
  Res res{1, req_id, reply_to, "", ""};
  try {
    if (kws.size() > 0) {
      spdlog::error("Invalid parameter passing mode<k: v>");
      throw std::runtime_error("Invalid parameter passing mode<k: v>");
    }
    
    if (auto search = rpc_func_map.find(meth); search != rpc_func_map.end()) {
      rpc_func_map[meth](&res, args);
    }
  } catch(const std::runtime_error &e) {
    spdlog::error("runtime error for request id {}\n{}", req_id, e.what());
    res.error = e.what();
    res.result = json::array({nullptr}).dump();
  } catch(const std::exception &e) {
    spdlog::error("BUG for request id {}", req_id);
    spdlog::error(e.what());
    throw e;
  }    
  reply(&res);
}

void MessageQueueServer::send_reply(const AMQP::Message *message, Res *res) {
  auto body = json::array({res->type, res->msgid, res->error, json::parse(res->result)[0]}).dump();
  AMQP::Envelope env(body);
  env.setCorrelationID(res->msgid);

  std::unique_lock<std::mutex> lock(send_mtx);
  send_channel->publish("", res->reply_to, env);
  spdlog::debug("sending reply [{}, {}])", res->reply_to, res->msgid);
}

void MessageQueueServer::unregister_rpc(const string &name) {
  if (auto search = rpc_func_map.find(name); search != rpc_func_map.end()) {
    rpc_func_map.erase(search);
    spdlog::info("unregister_rpc: {}", name);
  }
}

void MessageQueueServer::unregister_event_handler(int cb_id) {
  string event_type{""};
  for (auto &[k, v] : event_handler_map) {
    if (v.find(cb_id) != v.end()) {
      event_type = k;
      break;
    }
  }
  if (event_type.empty()) return;
  event_handler_map[event_type].erase(cb_id);
  if (event_handler_map.find(event_type) == event_handler_map.end()) {
    event_handler_map.erase(event_type);
  }
  spdlog::info("unregister_event_handler: {} {}", event_type, cb_id);
}

MessageQueueServer make_server(string conn, string rpc_routing_key, string event_routing_key, string rpc_exchange, string event_exchange) {
  get_connection(conn);
  get_rpc_exchange(rpc_exchange);
  get_event_exchange(event_exchange);
  return MessageQueueServer(conn, rpc_routing_key, event_routing_key, rpc_exchange, event_exchange);
}

void wait_conn(string addr, u_int port, u_int max_tries) {
  for (int i=0; i<max_tries; ++i) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
      spdlog::error("Failed to create socket.");
      sleep(2^i);
      continue;
    }
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    if (addr == "localhost")
      addr = "127.0.0.1";
    server_addr.sin_addr.s_addr = inet_addr(addr.c_str());
    server_addr.sin_port = htons(port);

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
      spdlog::error("Failed to connect.");
      sleep(2^i);
      continue;
    }
    close(server_socket);
    break;
  }
}

void run_server(MessageQueueServer &server, bool block, u_int max_tries) {
  string conn = server.conn_n;
  string conn_sp = split_str(conn, '@')[1];
  string addr = split_str(conn_sp, ':')[0];
  string p = split_str(split_str(conn_sp, ':')[1], '/')[0];
  u_int port = std::stoul(p);
  spdlog::info("starting at {}", conn);
  wait_conn(addr, port, max_tries);
  
  server.setup();

  std::thread run_t([&server]{server.run();});

  if (block) {
    while (true) {
      try {
        run_t.join();
      } catch(...) {
        spdlog::error("exit");
        break;
      }
    }
  } else {
    // run_t.detach();
    return;
  }
}

}  // namespace ccmqsrv