#pragma once
#include <event2/event.h>
#include <functional>
#include <unistd.h>
#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>
#include <amqpcpp/libevent.h>
#include <spdlog/spdlog.h>

/**
 * @brief inherit from AMQP::LibEventHandler, override some methods to handle connection error and disable heartbeat 
 */
class MQEventHandler : public AMQP::LibEventHandler
{
public:
  /**
   * @brief Construct a new MQEventHandler object
   * 
   * @param evbase event_base object
   */
  MQEventHandler(struct event_base* evbase) : LibEventHandler(evbase), evbase_(evbase_) {}
  
  /**
   * @brief override onError, log error message and break event loop
   * 
   * @param connection AMQP::TcpConnection object
   * @param message error message
   */
  void onError(AMQP::TcpConnection *connection, const char *message) override
  {
    spdlog::error(message);
    event_base_loopbreak(evbase_);
  }

  /**
   * @brief override onNegotiate, disable heartbeat
   * 
   * @param connection AMQP::TcpConnection object
   * @param interval heartbeat interval in seconds
   * @return uint16_t 0 to disable heartbeat
   */
  uint16_t onNegotiate(AMQP::TcpConnection *connection, uint16_t interval) override
  {
    // 取消heartbeat, 避免因为心跳检测导致的连接断开
    return 0;
  }

private:
  struct event_base* evbase_ {nullptr};
};

/**
 * @brief Connection handler, create event_base and event for stdin, and use MQEventHandler as AMQP::TcpHandler
 */
class ConnHandler
{
public:
  // using std::unique_ptr to manage event_base and event object
  using EventBasePtrT = std::unique_ptr<struct event_base, std::function<void(struct event_base*)>>;
  using EventPtrT = std::unique_ptr<struct event, std::function<void(struct event*)>>;

  /**
   * @brief Construct a new Conn Handler object and create event_base and event for stdin
   */
  ConnHandler()
    : evbase_(event_base_new(), event_base_free)
    , stdin_event_(event_new(evbase_.get(), STDIN_FILENO, EV_READ, stop, evbase_.get()), event_free)
    , evhandler_(evbase_.get())
  {
    event_add(stdin_event_.get(), nullptr);
  }

  /**
   * @brief Start event loop
   */
  void loop()
  {
    event_base_dispatch(evbase_.get());
  }
  
  /**
   * @brief Break event loop
   */
  void loop_break()
  {
    event_base_loopbreak(evbase_.get());
  }

  /**
   * @brief Get AMQP::TcpHandler object for connection
   * 
   * @return AMQP::TcpHandler* 
   */
  operator AMQP::TcpHandler*()
  {
    return &evhandler_;
  }

private:
  /**
   * @brief Static function to stop event loop when stdin is closed
   */
  static void stop(evutil_socket_t fd, short what, void *evbase)
  {
    event_base_loopbreak(reinterpret_cast<event_base*>(evbase));
  }
  EventBasePtrT evbase_;
  EventPtrT stdin_event_;
  MQEventHandler evhandler_;
};
