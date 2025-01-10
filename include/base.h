#pragma once
#include <string>
#include <spdlog/spdlog.h>
#include "amqp_handler.h"

#define RPC_EXCHANGE "mqsrv_rpc_exchange"
#define EVT_EXCHANGE "mqsrv_evt_exchange"
#define CONNECTION "amqp://guest:guest@localhost:5672//"

namespace ccmqsrv
{
using std::string;

void get_rpc_exchange(string &name);

void get_event_exchange(string &name);

void get_connection(string &conn);
}  // namespace ccmqsrv