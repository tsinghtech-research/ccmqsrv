#include "server.h"

using namespace ccmqsrv;

int add(int a, int b)
{
  spdlog::info("add({}, {})", a, b);
  return a+b;
}

std::string test_str(const std::string& str)
{
  spdlog::info("test_str({})", str);
  return "test_str: " + str;
}

void foo()
{
  std::cout << "foo" << std::endl;
}

std::vector<int> test_vec()
{
  spdlog::info("test_vec");
  std::vector<int> res{1, 2, 3, 4, 5};
  return res;
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
  run_server(server);
  return 0;
}