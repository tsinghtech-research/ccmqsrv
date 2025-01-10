#include <iostream>
#include <exception>
#include <string>
#include <execinfo.h>
#include <stdlib.h>

class BackTraceException : public std::exception {
public:
  BackTraceException(const std::string& msg);
  virtual ~BackTraceException() throw();
  const char* what() const throw() override;
  const char* backtrace() const throw();

private:
  void fill_stacktrace();

private:
  std::string msg_;
  std::string stack_;
};

BackTraceException::BackTraceException(const std::string& msg) : msg_(msg) {
  fill_stacktrace();
}

BackTraceException::~BackTraceException() throw() {}

const char* BackTraceException::what() const throw() {
  return msg_.c_str();
}

const char* BackTraceException::backtrace() const throw() {
  return stack_.c_str();
}

void BackTraceException::fill_stacktrace() {
  const int len = 200;
  void* buffer[len];
  int nptrs = ::backtrace(buffer, len);
  char** strings = ::backtrace_symbols(buffer, nptrs);
  if (strings)
  {
    for (int i = 0; i < nptrs; ++i)
    {
      stack_.append(strings[i]);
      stack_.push_back('\n');
    }
    free(strings);
  }
}

int main() {
  try {
    throw BackTraceException("test exception");
  } catch (BackTraceException& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << e.backtrace() << std::endl;
  }
  return 0;
} 