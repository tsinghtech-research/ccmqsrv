add_library(ccmqsrv
    "base.cc"
    "common.cc"
    "config.cc"
    "exc.cc"
    "server.cc"
    "client.cc"
    "rpc_utils.cc"
)
target_include_directories(ccmqsrv PUBLIC ${INC_DIR})
target_link_libraries(ccmqsrv PUBLIC spdlog::spdlog event amqpcpp fmt nlohmann_json::nlohmann_json uuid)
