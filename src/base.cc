#include "base.h"

namespace ccmqsrv {
  
void get_rpc_exchange(string &name) {
  if (name.empty())
    name = RPC_EXCHANGE;
}

void get_event_exchange(string &name) {
  if (name.empty())
    name = EVT_EXCHANGE;
}

void get_connection(string &conn) {
  if (conn.empty())
    conn = CONNECTION;
};
}  // namespace ccmqsrv