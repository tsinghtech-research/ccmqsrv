#include <iostream>
#include <toml.hpp>
#include <spdlog/spdlog.h>

int main()
{
  toml::table t = toml::get<toml::table>(toml::parse("/home/shenj/projects/ccmqsrv/tests/example.toml"));

  std::cout << "Name: " << t["name"] << std::endl;
  std::cout << "Version: " << t["version"] << std::endl;
  std::cout << "Compiler Name: " << t["compiler"]["name"] << std::endl;
  std::cout << "Compiler Version: " << t["compiler"]["version"] << std::endl;
  return 0;
}