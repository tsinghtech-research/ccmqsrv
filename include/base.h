#pragma once
#include <string>
#include <spdlog/spdlog.h>
#include "amqp_handler.h"

#ifndef RPC_EXCHANGE
#define RPC_EXCHANGE "mqsrv_rpc_exchange"
#endif

#ifndef EVT_EXCHANGE
#define EVT_EXCHANGE "mqsrv_evt_exchange"
#endif

#ifndef CONNECTION
#define CONNECTION "amqp://guest:guest@localhost:5672//"
#endif

namespace ccmqsrv
{
using std::string;

/**
 * @brief Get the rpc exchange name
 * 
 * @param name ref to the exchange name, will be set to the default value if not set
 */
void get_rpc_exchange(string &name);

/**
 * @brief Get the event exchange name
 * 
 * @param name ref to the exchange name, will be set to the default value if not set
 */
void get_event_exchange(string &name);

/**
 * @brief Get the connection of the message queue name
 * 
 * @param conn ref to the connection string, will be set to the default value if not set
 */
void get_connection(string &conn);
}  // namespace ccmqsrv