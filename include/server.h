#pragma once
#include <functional>
#include <unordered_map>
#include "base.h"
#include "rpc_utils.h"
#include "thread_pool.h"

namespace ccmqsrv
{
using namespace std::placeholders; // 使用占位符

struct Res {
  int type = 1;
  string msgid;
  string reply_to;
  string result;
  string error;
};

class MessageQueueServer
{
public:
  MessageQueueServer(string connection, string rpc_queue, string event_queue, string rpc_exchange, string event_exchange, u_int pool_size=1000)
  : conn_n(connection), rpc_queue_n(rpc_queue), event_queue_n(event_queue), rpc_exchange_n(rpc_exchange), event_exchange_n(event_exchange), pool_size_(pool_size) {};
  ~MessageQueueServer() {teardown();};

  void setup();
  void teardown();
  void run();

  template<typename Func>
  void register_rpc(const Func &func, const string &name);

  template<typename Func, typename Self>
  void register_rpc(const Func &func, Self *self, const string &name);
 
  void unregister_rpc(const string &name);

  template<typename CallBack>
  void register_event_handler(string event_type, CallBack cb);

  template<typename CallBack, typename Self>
  void register_event_handler(string event_type, Self *self, CallBack cb);

  void unregister_event_handler(int cb_id);

  template<typename CallBack>
  int register_exc_handler(CallBack exc_cb);

  void unregister_exc_handler(int exc_id);

private:
  void _on_rpc_message(const AMQP::Message &message, const string &req_id, const string &reply_to);
  void _rpc_worker(const AMQP::Message &message, const string &req_id, const string &reply_to);
  void _on_event_message(const AMQP::Message &message);
  void _event_worker(const string &event_type, int cb_id, const std::any &event_data);
  void send_reply(const AMQP::Message *message, Res *res);

public:
  string conn_n{""};
  string rpc_queue_n{""};
  string event_queue_n{""};
  string rpc_exchange_n{""};
  string event_exchange_n{""};
  u_int pool_size_{1000};

private:
  AMQP::TcpChannel *channel;
  ThreadPool ctx_pool{(int)pool_size_};
  ThreadPool pool{(int)pool_size_};
  bool is_setuped{false};
  
  // event
  int evt_cb_id{0};
  std::unordered_map<string, std::map<int, std::function<void(Res*, std::vector<std::any>&)>>> event_handler_map;
  // rpc
  // 绑定函数名和函数
  std::unordered_map<string, std::function<void(Res*, std::vector<std::any>&)>> rpc_func_map;
};

MessageQueueServer make_server(string conn="", string rpc_routing_key="", string event_routing_key="", string rpc_exchange="", string event_exchange="");
void run_server(MessageQueueServer &server, bool block=true, u_int max_tries=10);

}  // namespace ccmqsrv

#include "server.inl"