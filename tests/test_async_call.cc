#include "client.h"

using namespace ccmqsrv;

int main()
{
  spdlog::set_level(spdlog::level::debug);
  auto client = make_client();
  auto caller = client.get_caller("test_rpc_queue");

  int count1{0};
  int count2{0};

  std::thread t1([&]{
    for (count1; count1<1000000; ++count1) {
    auto res = caller("add", 1, 2); 
    spdlog::info("count1: {}", count1); 
    }});
  std::thread t2([&]{
    for (count2; count2<1000000; ++count2) {
    auto res = caller("test_str", "Nirvana");
    spdlog::info("count2: {}", count2); 
    }});

  t1.join();
  t2.join();
  std::cout << "count1: " << count1 << "\n";
  std::cout << "count2: " << count2 << "\n";
  return 0;
}