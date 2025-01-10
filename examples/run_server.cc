#include "server.h"

using namespace ccmqsrv;

void foo()
{
  std::cout << "foo" << std::endl;
}

int add(int a, int b)
{
  spdlog::info("add({}, {})", a, b);
  // sleep(2);
  return a+b;
}

std::string test_str(const std::string& str)
{
  spdlog::info("test_str({})", str);
  return "test_str: " + str;
}

std::vector<int> test_vec(std::vector<int> vec)
{
  spdlog::info("test_vec");
  for (auto& i : vec) {
    spdlog::info("{}", i);
  }
  std::vector<int> res{1, 2, 3, 4, 5};
  return res;
}

std::map<std::string, int> test_map(std::map<std::string, int> m)
{
  spdlog::info("test_map");
  for (auto& [key, value] : m) {
    spdlog::info("{} : {}", key, value);
  }
  std::map<std::string, int> res{{"a", 1}, {"b", 2}, {"c", 3}};   
  return res;
}

void test_err()
{
  throw std::runtime_error("test_err");
}

class Test {
public:
  int sub(int a, int b)
  {
    spdlog::info("sub({}, {})", a, b);
    return a-b;
  }
};

int main()
{
  spdlog::set_level(spdlog::level::debug);
  auto server = make_server("", "test_rpc_queue");
  Test test;
  server.register_rpc(&Test::sub, &test, "sub");
  server.register_rpc(add, "add");
  server.register_rpc(foo, "foo");
  server.register_rpc(test_str, "test_str");
  server.register_rpc(test_vec, "test_vec");
  server.register_rpc(test_map, "test_map");
  server.register_rpc(test_err, "test_err");
  run_server(server);
  return 0;
}