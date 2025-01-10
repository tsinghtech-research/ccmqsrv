#pragma once
#include <string>

/**
 * @brief Load config from file, toml or yaml.
 * 
 * @param path config file path
 * @return decltype(auto) parsed data
 */
decltype(auto) load_config(const std::string& path);