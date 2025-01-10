#include <cassert>
#include <map>
#include <sstream>
#include "rpc_utils.h"

namespace ccmqsrv {

std::any _parse_json_element(const json& element)
{
  if (element.is_object()) {
    // object类型
    std::map<string, std::any> obj;
    for (auto& [key, value]: element.items()) {
      obj[key] = _parse_json_element(value);
    }
    return obj;
  } 
  else if (element.is_array()) {
    // list tuple类型
    std::vector<std::any> array;
    for (const auto& nested_element : element) {
      array.push_back(_parse_json_element(nested_element));
    }
    return array;
  } 
  else if (element.is_string()) {
    return element.get<string>();
  } 
  else if (element.is_number_integer()) {
    return element.get<int>();
  } 
  else if (element.is_number_float()) {
    return element.get<double>();
  } 
  else if (element.is_boolean()) {
    return element.get<bool>();
  } 
  else if (element.is_null()) {
    return nullptr;
  } 
  else {
    return nullptr;
  }
}

std::vector<std::any> json_loadb(string json_string)
{
  json json_data = json::parse(json_string);
  std::vector<std::any> parse_vec;
  for (const auto& element: json_data)
  {
    parse_vec.emplace_back(_parse_json_element(element));
  }
  return parse_vec;
}

void rpc_decode_req(std::vector<std::any> &data, string &msgid, string &meth, std::vector<std::any> &args, std::map<string, std::any> &kws, string check_id)
{
  const int typ = std::any_cast<int>(data[0]);
  assert(typ==0);
  msgid = std::any_cast<string>(data[1]);
  if (!check_id.empty())
    assert(msgid==check_id);
  meth = std::any_cast<string>(data[2]);
  args = std::any_cast<std::vector<std::any>>(data[3]);
  kws = std::any_cast<std::map<string, std::any>>(data[4]);
}

void rpc_decode_rep(const string &data, string &error, string &result, string check_id)
{
  json json_data = json::parse(data);
  int typ = json_data[0].get<int>();
  assert(typ==1);
  string msgid = json_data[1].get<string>();
  if (!check_id.empty())
    assert(msgid==check_id);
  
  if (!json_data[2].is_null() && json_data[2].is_string())
    error = json_data[2].get<string>();
  result = json_data[3].dump();
}

}  // namespace ccmqsrv
