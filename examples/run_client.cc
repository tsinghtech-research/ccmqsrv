#include <iostream>
#include "client.h"

using namespace ccmqsrv;

int main()
{
  std::cout << "run client." << std::endl;
  spdlog::set_level(spdlog::level::debug);
  auto client = make_client();
  auto caller = client.get_caller("test_rpc_queue");

  for (int i = 0; i < 1000000; i++) {
  // timeout call (ms)
  auto res_add_1 = caller(3000, "add", 1, 2);
  std::cout << "add res: " << res_add_1 << std::endl;
  // no timeout call
  auto res_add_2 = caller("add", 1, 2);
  std::cout << "add res: " << res_add_2 << std::endl;

  auto res_foo = caller("foo");
  std::cout << "foo res: " << res_foo << std::endl;

  auto res_sub = caller("sub", 3, 4);
  std::cout << "sub res: " << res_sub << std::endl;

  auto res_str = caller("test_str", "Nirvana");
  std::cout << "test_str res: " << res_str << std::endl;

  std::vector<int> vec = {1, 2, 3};
  auto res_vec = caller("test_vec", vec);
  for (auto& x : res_vec) {
    std::cout << "test_vec res: " << x << std::endl;
  }

  std::map<std::string, int> map = {{"a", 1}, {"b", 2}, {"c", 3}};
  std::map<std::string, int> res_map = caller("test_map", map);
  for (auto& [k, v] : res_map) {
    std::cout << "test_map res: " << k << ":" << v << std::endl;
  }
  }
  // caller("test_err");

  return 0;
}