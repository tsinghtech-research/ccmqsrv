#pragma once
#include <vector>
#include <string>

/**
 * @brief split a string by a delimiter
 * 
 * @param str original string
 * @param delimiter splitting delimiter
 * @return std::vector<std::string> splitted string
 */
std::vector<std::string> split_str(const std::string& str, char delimiter);
