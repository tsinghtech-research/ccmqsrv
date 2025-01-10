#pragma once
#include <event2/event.h>
#include <functional>
#include <unistd.h>
#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>
#include <amqpcpp/libevent.h>
#include <spdlog/spdlog.h>

class LibEventHandlerMyError : public AMQP::LibEventHandler
{
public:
  LibEventHandlerMyError(struct event_base* evbase) : LibEventHandler(evbase), evbase_(evbase_) {}
  void onError(AMQP::TcpConnection *connection, const char *message) override
  {
    spdlog::error(message);
    event_base_loopbreak(evbase_);
  }
  uint16_t onNegotiate(AMQP::TcpConnection *connection, uint16_t interval) override
  {
    // 取消heartbeat, 避免因为心跳检测导致的连接断开
    return 0;
  }

private:
  struct event_base* evbase_ {nullptr};
};

class ConnHandler
{
public:
  using EventBasePtrT = std::unique_ptr<struct event_base, std::function<void(struct event_base*)> >;
  using EventPtrT = std::unique_ptr<struct event, std::function<void(struct event*)> >;

  ConnHandler()
    : evbase_(event_base_new(), event_base_free)
    , stdin_event_(event_new(evbase_.get(), STDIN_FILENO, EV_READ, stop, evbase_.get()), event_free)
    , evhandler_(evbase_.get())
  {
    event_add(stdin_event_.get(), nullptr);
  }

  void loop()
  {
    event_base_dispatch(evbase_.get());
  }
  
  void loop_break()
  {
    event_base_loopbreak(evbase_.get());
  }

  operator AMQP::TcpHandler*()
  {
    return &evhandler_;
  }

private:
  static void stop(evutil_socket_t fd, short what, void *evbase)
  {
    event_base_loopbreak(reinterpret_cast<event_base*>(evbase));
  }
  EventBasePtrT evbase_;
  EventPtrT stdin_event_;
  LibEventHandlerMyError evhandler_;
};
