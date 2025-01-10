#include <iostream>
#include <execinfo.h>
#include <stdio.h>
#include "exc.h"

namespace ccmqsrv { 

std::map<std::string, std::any> pack_exc(std::vector<std::any> &error) {
  std::any et = error[0], ev = error[1], tb = error[2];
  void *callstack[128];
  int frames = backtrace(callstack, 128);
  char** strs = backtrace_symbols(callstack, frames);

  std::map<std::string, std::any> out;

  free(strs);
  return out;
}

std::map<std::string, std::any> BaseException::asdict() {
  std::map<std::string, std::any> out{{"code", this->_code}, {"message", this->_msg}, {"data", this->_data}};
  return out;
}

} // namespace ccmqsrv
