#pragma once
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace ccmqsrv {
using std::string;
using json = nlohmann::json;

/**
 * @brief parse json string to std::vector<std::any>
 * 
 * @param json_string json string
 * @return std::vector<std::any> parsed result
 */
std::vector<std::any> json_loadb(string json_string);

/**
 * @brief encode rpc request message
 * 
 * @tparam Args method arguments types
 * @param msgid request message id
 * @param meth method name
 * @param args method arguments
 * @return string encoded message
 */
template<typename... Args>
string rpc_encode_req(string &msgid, string &meth, Args&&... args) {
  return json::array({0, msgid, meth, json::array({std::forward<Args>(args)...}), json::object()}).dump();
}

/**
 * @brief decode rpc request message
 * 
 * @param data rpc request message
 * @param msgid message id
 * @param meth method name
 * @param args method arguments
 * @param kws method keyword arguments
 * @param check_id request message id to check, if not empty, check if msgid is equal to check_id
 */
void rpc_decode_req(std::vector<std::any> &data, string &msgid, string &meth, std::vector<std::any> &args, std::map<string, std::any> &kws, string check_id="");

/**
 * @brief decode rpc response message
 * 
 * @param data rpc response message
 * @param error method call res error message
 * @param result method call result
 * @param check_id response message id to check, if not empty, check if msgid is equal to check_id
 */
void rpc_decode_rep(const string &data, string &error, string &result, string check_id="");
}  // namespace ccmqsrv