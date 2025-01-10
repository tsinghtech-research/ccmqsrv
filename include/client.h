#pragma once
#include <uuid/uuid.h>
#include "base.h"
#include "rpc_utils.h"

namespace ccmqsrv {
class MessageQueueClient;

class AsyncResult {
public:
  // 设置异步操作的结果
  template<typename... Args>
  void set(std::tuple<Args...> &&result);

  template<typename... Args>
  void set(std::tuple<Args...> &result);

  // 获取异步操作的结果
  std::tuple<string, string, string>&& get(int timeout=1000);

  // 等待异步操作完成
  void wait();

private:
  std::mutex mutex_;
  std::condition_variable cond_var_;
  bool done_ = false;

  // ret: (req_id, error, result)
  std::tuple<string, string, string> result_;
};

class Publisher {
public:
  Publisher(MessageQueueClient *client, string routing_key) : client_(client), routing_key_(routing_key) {};
  ~Publisher() {};

  void operator()(string evt_type, auto&& event_data);

private:
  MessageQueueClient *client_;
  string routing_key_;
};

class Caller {
public:
  Caller(MessageQueueClient *client, string routing_key) : client_(client), routing_key_(routing_key) {};
  ~Caller() {};

  decltype(auto) operator()(int timeout=1000, string meth="", auto&&... args); 
  decltype(auto) operator()(string meth="", auto&&... args);

private:
  MessageQueueClient *client_;
  string routing_key_;

  decltype(auto) call(int timeout=1000, string meth="", auto&&... args);
};

class MessageQueueClient {
public:
  MessageQueueClient(string conn, string rpc_exchange, string callback_queue, string event_exchange) : 
    conn_(conn), rpc_exchange_(rpc_exchange), callback_queue_(callback_queue), event_exchange_(event_exchange) {
      run_t = std::thread(&MessageQueueClient::run, this);
      run_t.detach();
      while (true) {if (is_running) break;}
    }
  ~MessageQueueClient() { release(); };

  Publisher get_pubber(string routing_key) {return Publisher(this, routing_key);}
  Caller get_caller(string routing_key) {return Caller(this, routing_key);}

  void release();

  void publish(string routing_key, string evt_type, auto&& event_data);
  
  template<typename... Args>
  decltype(auto) call_async(string routing_key, string meth, Args&&... args);

public:
  string conn_;
  string rpc_exchange_;
  string callback_queue_;
  string event_exchange_;
  std::map<string, std::shared_ptr<AsyncResult>> req_events;

private:
  bool is_running = false;

  std::mutex pub_mtx;
  std::condition_variable pub_cv;
  bool pub_queue_inited = false;
  // routing key vec
  std::set<string> routing_keys_;
  
  bool should_stop = false;
  AMQP::TcpChannel *channel;

  std::thread run_t;
  void run();

  void on_response(const AMQP::Message &message);
};

MessageQueueClient make_client(string conn="", string rpc_exchange="", string callback_queue="", string event_exchange="");

}  // namespace ccmqsrv

#include "client.inl"