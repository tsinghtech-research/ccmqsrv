#pragma once
#include <any>
#include <map>
#include <vector>
#include <string>
#include <exception>

namespace ccmqsrv {
std::map<std::string, std::any> pack_exc(std::vector<std::any> &error);


class BaseException: public std::exception {
public:
  BaseException(int code, std::string &msg, std::map<std::any, std::any> &data): _code(code), _msg(msg), _data(data) {}
  ~BaseException() {}

  std::map<std::string, std::any> asdict();

private:
  int _code = -1;
  std::string _msg = "";
  std::map<std::any, std::any> _data;
};

} // namespace ccmqsrv