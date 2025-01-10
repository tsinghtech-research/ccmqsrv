#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <uuid/uuid.h>
#include "base.h"
#include "rpc_utils.h"

namespace ccmqsrv {
class MessageQueueClient;

/**
 * @brief Call Function Result set and get.
 */
class AsyncResult {
public:
  // 设置异步操作的结果
  template<typename... Args>
  void set(std::tuple<Args...> &&result);

  template<typename... Args>
  void set(std::tuple<Args...> &result);

  // 获取异步操作的结果
  std::tuple<string, string>&& get(int timeout=1000);

  // 等待异步操作完成
  void wait();

private:
  std::mutex mutex_;
  std::condition_variable cond_var_;
  bool done_ = false;

  // ret: (error, result)
  std::tuple<string, string> result_;
};

/**
 * @brief event publisher
 */
class Publisher {
public:
  Publisher(MessageQueueClient *client, string routing_key) : client_(client), routing_key_(routing_key) {};
  ~Publisher() {};

  /**
   * @brief pub event
   * 
   * @param evt_type 
   * @param event_data 
   */
  void operator()(string evt_type, auto&& event_data);

private:
  MessageQueueClient *client_;
  string routing_key_;
};

/**
 * @brief rpc caller
 */
class Caller {
public:
  Caller(MessageQueueClient *client, string routing_key) : client_(client), routing_key_(routing_key) {};
  ~Caller() {};

  /**
   * @brief rpc call
   * 
   * @param timeout set function call timeout, default 1000ms
   * @param meth method name, default ""
   * @param args method args, default empty tuple
   * @return decltype(auto) 
   */
  decltype(auto) operator()(int timeout=1000, string meth="", auto&&... args); 
  
  /**
   * @brief rpc call, without timeout
   * 
   * @param meth method name, default ""
   * @param args method args, default empty tuple
   * @return decltype(auto) 
   */
  decltype(auto) operator()(string meth="", auto&&... args);

private:
  MessageQueueClient *client_;
  string routing_key_;

  /**
   * @brief private rpc call
   * 
   * @param timeout set function call timeout, default 1000ms
   * @param meth method name, default ""
   * @param args method args, default empty tuple
   * @return decltype(auto) 
   */
  decltype(auto) call(int timeout=1000, string meth="", auto&&... args);
};

/**
 * @brief MessageQueueClient
 */
class MessageQueueClient {
public:
  /**
   * @brief Construct a new Message Queue Client object
   * 
   * @param conn connection addr, like amqp://guest:guest@localhost:5672//
   * @param rpc_exchange rpc exchange name
   * @param callback_queue callback queue name
   * @param event_exchange event exchange name
   */
  MessageQueueClient(string conn, string rpc_exchange, string callback_queue, string event_exchange) : 
    conn_(conn), rpc_exchange_(rpc_exchange), callback_queue_(callback_queue), event_exchange_(event_exchange) {
      std::thread run_t([&]{run();});
      run_t.detach();
      while (true) {if (is_running) break;}
    }

  /**
   * @brief copy constructor
   * 
   * @param client other client
   */
  MessageQueueClient(const MessageQueueClient& client) : 
    conn_(client.conn_), rpc_exchange_(client.rpc_exchange_), callback_queue_(client.callback_queue_), event_exchange_(client.event_exchange_) {};
  
  /**
   * @brief move constructor
   * 
   * @param client other client
   */
  MessageQueueClient(MessageQueueClient&& client) : 
    conn_(std::move(client.conn_)), rpc_exchange_(std::move(client.rpc_exchange_)), callback_queue_(std::move(client.callback_queue_)), event_exchange_(std::move(client.event_exchange_)) {};
  
  /**
   * @brief copy assignment operator
   * 
   * @param client other client
   * @return MessageQueueClient& return ref to self
   */
  MessageQueueClient& operator=(const MessageQueueClient& client) {
    conn_ = client.conn_;
    rpc_exchange_ = client.rpc_exchange_;
    callback_queue_ = client.callback_queue_;
    event_exchange_ = client.event_exchange_;
    return *this;
  }

  /**
   * @brief Destroy the Message Queue Client object
   * 
   * release resources
   * 
   */
  virtual ~MessageQueueClient() { release(); };

  /**
   * @brief Get the pubber object
   * 
   * @param routing_key routing key for publisher name
   * @return Publisher publisher object
   */
  Publisher get_pubber(string routing_key) { return Publisher(this, routing_key); }
  
  /**
   * @brief Get the caller object
   * 
   * @param routing_key routing key for caller name
   * @return Caller caller object
   */
  Caller get_caller(string routing_key) { return Caller(this, routing_key); }

  /**
   * @brief release resources
   * 
   */
  void release();

  /**
   * @brief publish event to event exchange
   * 
   * @param routing_key routing key for publisher name
   * @param evt_type publish event type
   * @param event_data pub data
   */
  void publish(string routing_key, string evt_type, auto&& event_data);
  
  /**
   * @brief call rpc method
   * 
   * @tparam Args method args type
   * @param routing_key routing key for caller name
   * @param meth method name
   * @param args method args
   * @return string method result json dump
   */
  template<typename... Args>
  string call_async(string routing_key, string meth, Args&&... args);

public:
  string conn_;
  string rpc_exchange_;
  string callback_queue_;
  string event_exchange_;
  std::map<string, std::shared_ptr<AsyncResult>> req_events;
  std::mutex req_events_mtx;

private:
  // 使用atomic保证线程安全, 如果此处使用基础bool类型可能导致release模式下引发Segmentation fault
  std::atomic_bool is_running = false;

  std::mutex pub_mtx;
  std::condition_variable pub_cv;
  bool pub_queue_inited = false;
  // routing key vec
  std::set<string> routing_keys_;
  
  bool should_stop = false;
  std::shared_ptr<AMQP::TcpChannel> send_channel;
  std::shared_ptr<AMQP::TcpChannel> recv_channel;
  std::mutex send_mtx;

  /**
   * @brief run message queue client
   * 
   * create connection, channel, and start message event loop
   * create send and recv channel, and start message loop
   * 
   * declare rpc exchange, callback queue, and event exchange
   * bind callback queue
   */
  void run();

  /**
   * @brief consume message from rpc exchange
   * 
   * @param message recv message object
   */
  void on_response(const AMQP::Message &message);
};

/**
 * @brief make message queue client object
 * 
 * @param conn connection addr, like amqp://guest:guest@localhost:5672//
 * @param rpc_exchange rpc exchange name
 * @param callback_queue callback queue name
 * @param event_exchange event exchange name
 * @return MessageQueueClient MessageQueueClient object
 */
MessageQueueClient make_client(string conn="", string rpc_exchange="", string callback_queue="", string event_exchange="");

}  // namespace ccmqsrv

#include "client.inl"