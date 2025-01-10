#pragma once
#include <any>
#include <map>
#include <vector>
#include <string>
#include <exception>

namespace ccmqsrv {
/**
 * @brief Pack the exception information into a map.
 * 
 * @param error error information
 * @return std::map<std::string, std::any> packed exception information
 */
std::map<std::string, std::any> pack_exc(std::vector<std::any> &error);

/**
 * @brief CCMQSRV exception base class
 */
class BaseException: public std::exception {
public:
  /**
   * @brief Construct a new Base Exception object
   * 
   * @param code error code
   * @param msg error message
   * @param data error data
   */
  BaseException(int code, std::string &msg, std::map<std::any, std::any> &data): _code(code), _msg(msg), _data(data) {}
  ~BaseException() {}

  /**
   * @brief error msg to map
   * 
   * @return std::map<std::string, std::any> error msg map
   */
  std::map<std::string, std::any> asdict();

private:
  int _code = -1;
  std::string _msg = "";
  std::map<std::any, std::any> _data;
};

} // namespace ccmqsrv