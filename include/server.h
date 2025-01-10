#pragma once
#include <functional>
#include <unordered_map>
#include <BS_thread_pool.hpp>
#include "base.h"
#include "rpc_utils.h"

namespace ccmqsrv
{
using namespace std::placeholders; // 使用占位符
using thread_pool = BS::thread_pool;

/**
 * @brief Call Func Result
 */
struct Res {
  int type = 1;
  string msgid;
  string reply_to;
  string result;
  string error;
};

/**
 * @brief MessageQueueServer
 */
class MessageQueueServer {
public:
  /**
   * @brief Construct a new Message Queue Server object
   * 
   * @param connection connection address
   * @param rpc_queue rpc queue name
   * @param event_queue event queue name
   * @param rpc_exchange rpc exchange name
   * @param event_exchange event exchange name
   * @param pool_size message queue thread pool size
   */
  MessageQueueServer(string connection, string rpc_queue, string event_queue, string rpc_exchange, string event_exchange, u_int pool_size=10)
  : conn_n(connection), rpc_queue_n(rpc_queue), event_queue_n(event_queue), rpc_exchange_n(rpc_exchange), event_exchange_n(event_exchange), pool_size_(pool_size) {};
  
  /**
   * @brief Destroy the MessageQueueServer object and release resources
   * 
   */
  virtual ~MessageQueueServer() { teardown(); };

  /**
   * @brief Setup the message queue server
   */
  void setup();

  /**
   * @brief Teardown the message queue server
   */
  void teardown();

  /**
   * @brief Run the message queue server
   * 
   * create connection, declare queue, bind exchange, start message queue thread pool, start event message thread
   */
  void run();

  /**
   * @brief Register rpc function, bind function to rpc_func_map
   * 
   * @tparam Func function type
   * @param func 
   * @param name function name
   */
  template<typename Func>
  void register_rpc(const Func &func, const string &name);

  /**
   * @brief Register rpc function, bind member function to rpc_func_map
   * 
   * @tparam Func function type
   * @tparam Self class type
   * @param func function
   * @param self class 
   * @param name function name
   */
  template<typename Func, typename Self>
  void register_rpc(const Func &func, Self *self, const string &name);
 
  /**
   * @brief erase rpc function from rpc_func_map
   * 
   * @param name function name
   */
  void unregister_rpc(const string &name);

  /**
   * @brief Register event handler, bind event type and callback func to event_handler_map
   * 
   * @tparam CallBack 
   * @param event_type event type
   * @param cb callback function
   */
  template<typename CallBack>
  void register_event_handler(string event_type, CallBack cb);

  /**
   * @brief Register event handler, bind member function to event_handler_map
   * 
   * @tparam CallBack function type
   * @tparam Self class type
   * @param event_type event type
   * @param self class
   * @param cb callback function
   */
  template<typename CallBack, typename Self>
  void register_event_handler(string event_type, Self *self, CallBack cb);

  /**
   * @brief Erase event handler from event_handler_map
   * 
   */
  void unregister_event_handler(int cb_id);

private:
  /**
   * @brief Process rpc message
   * 
   * @param message AMQP::Message object
   * @param body message body
   * @param req_id request id
   * @param reply_to reply to queue name
   */
  void _on_rpc_message(const AMQP::Message &message, const string &body, const string &req_id, const string &reply_to);
  
  /**
   * @brief Real call rpc function
   * 
   * @param message AMQP::Message object
   * @param body message body
   * @param req_id request id
   * @param reply_to reply to queue name
   */
  void _rpc_worker(const AMQP::Message &message, const string &body, const string &req_id, const string &reply_to);
  
  /**
   * @brief Process event message
   * 
   * @param message AMQP::Message object
   */
  void _on_event_message(const AMQP::Message &message);
  
  /**
   * @brief event message worker
   * 
   * @param event_type event type
   * @param cb_id callback id
   * @param event_data event data
   */
  void _event_worker(const string &event_type, int cb_id, const std::any &event_data);
  
  /**
   * @brief rpc func result pub to reply queue
   * 
   * @param message AMQP::Message object
   * @param res func result
   */
  void send_reply(const AMQP::Message *message, Res *res);

public:
  string conn_n{""};
  string rpc_queue_n{""};
  string event_queue_n{""};
  string rpc_exchange_n{""};
  string event_exchange_n{""};
  u_int pool_size_{10};

private:
  std::shared_ptr<AMQP::TcpChannel> recv_channel;
  std::shared_ptr<AMQP::TcpChannel> send_channel;
  std::mutex send_mtx;
  thread_pool pool{pool_size_};
  bool is_setuped{false};
  
  // event
  int evt_cb_id{0};
  std::unordered_map<string, std::map<int, std::function<void(Res*, std::vector<std::any>&)>>> event_handler_map;
  // rpc
  // 绑定函数名和函数
  std::unordered_map<string, std::function<void(Res*, std::vector<std::any>&)>> rpc_func_map;
};

/**
 * @brief Make a message queue server object
 * 
 * @param conn connection address
 * @param rpc_routing_key rpc routing key
 * @param event_routing_key event routing key
 * @param rpc_exchange rpc exchange name
 * @param event_exchange event exchange name
 * @return MessageQueueServer 
 */
MessageQueueServer make_server(string conn="", string rpc_routing_key="", string event_routing_key="", string rpc_exchange="", string event_exchange="");

/**
 * @brief run message queue server, block until stop or max_tries
 * 
 * @param server MessageQueueServer object
 * @param block join thread
 * @param max_tries connect max tries
 */
void run_server(MessageQueueServer &server, bool block=true, u_int max_tries=10);

}  // namespace ccmqsrv

#include "server.inl"