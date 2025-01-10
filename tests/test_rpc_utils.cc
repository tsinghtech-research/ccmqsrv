#include <iostream>
#include "rpc_utils.h"

int main() {
  using namespace ccmqsrv;
  std::string json_string = R"([0, "corr-ff32ff9f-7281-4fc5-a502-a2e94acf37d2", "echo", [1, "ss", [1, 2, "ss", 4, 2.3], 5.7], {"v": 5, "s": "aa"}])";
  auto parse_vec = json_loadb(json_string);
  std::cout << parse_vec.size() << std::endl;
  std::cout << std::any_cast<int>(parse_vec[0]) << std::endl;
  std::cout << std::any_cast<string>(parse_vec[1]) << std::endl;
  std::cout << std::any_cast<string>(parse_vec[2]) << std::endl;
  const std::vector<std::any>& nested_array = std::any_cast<std::vector<std::any>>(parse_vec[3]);
  const std::map<std::string, std::any>& nested_object = std::any_cast<std::map<std::string, std::any>>(parse_vec[4]);
  std::cout << nested_array.size() << std::endl;
  std::cout << nested_object.size() << std::endl;

  
}