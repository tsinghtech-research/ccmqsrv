#include "rpc_utils.h"

namespace ccmqsrv {
void Publisher::operator()(string evt_type, auto&& event_data) {
  client_->publish(routing_key_, evt_type, std::forward<decltype(event_data)>(event_data));
};

decltype(auto) Caller::call(int timeout, string meth, auto&&... args) {
  if (meth.empty())
    throw std::runtime_error("method name is empty");
  auto rets = client_->call_async(routing_key_, meth, std::forward<decltype(args)>(args)...);
  auto ret = rets->get(timeout);
  client_->req_events.erase(std::get<0>(ret));
  string error = std::get<1>(ret);
  if (!error.empty()) {
    throw std::runtime_error(error);
  }
  string result = std::get<2>(ret);
  
  return json::parse(result);
};

decltype(auto) Caller::operator()(string meth, auto&&... args) {
  // timeout 设置为 10 天
  int timeout = 1000*60*60*24*10;
  return call(timeout, meth, std::forward<decltype(args)>(args)...);
}

decltype(auto) Caller::operator()(int timeout, string meth, auto&&... args) {
  return call(timeout, meth, std::forward<decltype(args)>(args)...);
}

inline void MessageQueueClient::publish(string routing_key, string evt_type, auto&& event_data)
{
  string pub_data = json::array({evt_type, event_data}).dump();
  if (routing_keys_.find(routing_key) == routing_keys_.end())
  {
    channel->declareQueue(routing_key, AMQP::durable);
    channel->bindQueue(event_exchange_, routing_key, routing_key)
      .onSuccess([&]{ pub_queue_inited = true; pub_cv.notify_all(); });
    routing_keys_.insert(routing_key);
  }
  
  std::unique_lock<std::mutex> lock(pub_mtx);
  if (!pub_queue_inited) {
    pub_cv.wait(lock);
  }
  channel->publish(event_exchange_, routing_key, pub_data);
}

template<typename... Args>
inline decltype(auto) MessageQueueClient::call_async(string routing_key, string meth, Args&&... args)
{
  uuid_t uuid;
  char uuid_str[37];
  uuid_generate_random(uuid);
  uuid_unparse(uuid, uuid_str);
  string req_id = "corr-"+(string)uuid_str;
  spdlog::debug("sending request: [{}, {}, {}] {}", routing_key, callback_queue_, req_id, meth);

  req_events[req_id] = std::make_shared<AsyncResult>();

  string req_body = rpc_encode_req(req_id, meth, std::forward<Args>(args)...);
  AMQP::Envelope env(req_body);
  env.setCorrelationID(req_id);
  env.setReplyTo(callback_queue_);
  channel->publish(rpc_exchange_, routing_key, env);
  return req_events[req_id];
}
}