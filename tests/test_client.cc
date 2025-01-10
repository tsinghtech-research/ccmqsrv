#include "client.h"
using namespace ccmqsrv;

int main() {
  spdlog::set_level(spdlog::level::debug);
  auto client = make_client();
  auto caller = client.get_caller("test_rpc_queue");

  auto res = caller(500000, "add", 1, 2);
  std::cout << "Result: " << res << std::endl;
  return 0;
}