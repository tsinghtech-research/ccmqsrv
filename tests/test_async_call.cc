#include "client.h"

using namespace ccmqsrv;

int main()
{
  spdlog::set_level(spdlog::level::debug);
  auto client = make_client();
  auto caller = client.get_caller("test_rpc_queue");

  auto client2 = make_client();
  auto caller2 = client.get_caller("test_rpc_queue");

  std::thread t1([&]{for (int i=0; i<1000; ++i) {auto res = caller("add", 1, 2); std::cout << i << ": " << res <<"\n";}});
  std::thread t2([&]{for (int i=0; i<1000; ++i) {auto res = caller2("test_str", "Nirvana"); std::cout << i << ": " << res <<"\n";}});

  t1.join();
  t2.join();
  return 0;
}