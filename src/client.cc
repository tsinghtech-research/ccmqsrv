#include <iostream>
#include <mutex>
#include <condition_variable>
#include "client.h"

namespace ccmqsrv {

template<typename... Args>
void AsyncResult::set(std::tuple<Args...> &&result) {
  std::unique_lock<std::mutex> lock(mutex_);
  result_ = std::move(result);
  done_ = true;
  cond_var_.notify_all();
}

template<typename... Args>
void AsyncResult::set(std::tuple<Args...> &result) {
  set(std::move(result));
}

std::tuple<string, string, string>&& AsyncResult::get(int timeout) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (!done_) {
    auto cv_status = cond_var_.wait_for(lock, std::chrono::milliseconds(timeout));
    if (cv_status == std::cv_status::timeout)
      throw std::runtime_error("call function timeout.");
  }
  done_ = false;
  return std::move(result_);
}

void AsyncResult::wait() {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_var_.wait(lock, [this] { return done_; });
}

void MessageQueueClient::on_response(const AMQP::Message &message)
{
  auto req_id = message.correlationID();
  spdlog::debug("receiving response [{}, {}]", callback_queue_, req_id);
  string body = message.body();
  auto rep_data = body.substr(0, message.bodySize());
  if (req_events.find(req_id) != req_events.end()) {
    string error{""};
    string result{""};
    rpc_decode_rep(rep_data, error, result);
    req_events[req_id]->set(std::tuple<string, string, string>(req_id, error, result));
  }
}

void MessageQueueClient::run()
{
  ConnHandler c_handler;

  AMQP::Address addr(conn_);
  AMQP::TcpConnection conn(c_handler, addr);

  channel = new AMQP::TcpChannel(&conn);

  channel->declareExchange(rpc_exchange_, AMQP::topic, AMQP::durable);
  channel->declareExchange(event_exchange_, AMQP::topic, AMQP::durable);
  
  channel->declareQueue(callback_queue_, AMQP::exclusive+AMQP::autodelete);
  channel->bindQueue(rpc_exchange_, callback_queue_, callback_queue_)
    .onSuccess([&]{
      is_running = true;
    });
  channel->consume(callback_queue_, AMQP::noack)
    .onReceived(
      [this](const AMQP::Message& message, uint64_t, bool) {
        on_response(message);
      }
    );
  c_handler.loop();
  conn.close();
}
  
void MessageQueueClient::release()
{
  if (should_stop)
    return;

  should_stop = true;
  channel->close();
  delete channel;
  channel = nullptr;
}

MessageQueueClient make_client(string conn, string rpc_exchange, string callback_queue, string event_exchange) 
{
  get_connection(conn);
  get_rpc_exchange(rpc_exchange);
  get_event_exchange(event_exchange);

  if (callback_queue.empty()) {
    uuid_t uuid;
    char uuid_str[37];
    uuid_generate_random(uuid);
    uuid_unparse(uuid, uuid_str);
    callback_queue = "cbq-"+(string)uuid_str;
  }
  return MessageQueueClient(conn, rpc_exchange, callback_queue, event_exchange);
}

}  // namespace ccmqsrv