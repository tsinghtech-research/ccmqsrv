#include "common.h"

std::vector<std::string> split_str(const std::string& str, char delimiter) {
  std::vector<std::string> result;
  std::string token;
  for (char c : str) {
    if (c == delimiter) {
      if (!token.empty()) {
        result.push_back(token);
        token.clear();
      }
    } else {
      token += c;
    }
  }
  if (!token.empty()) {
    result.push_back(token);
  }
  return result;
}
