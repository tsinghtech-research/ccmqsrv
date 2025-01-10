#pragma once
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace ccmqsrv {
using std::string;
using json = nlohmann::json;

// json json_dumpb(auto&& v);
std::vector<std::any> json_loadb(string json_string);

template<typename... Args>
string rpc_encode_req(string &msgid, string &meth, Args&&... args)
{
  return json::array({0, msgid, meth, json::array({std::forward<Args>(args)...}), json::object()}).dump();
}

void rpc_decode_req(std::vector<std::any> &data, string &msgid, string &meth, std::vector<std::any> &args, std::map<string, std::any> &kws, string check_id="");

void rpc_decode_rep(const string &data, string &error, string &result, string check_id="");
}  // namespace ccmqsrv