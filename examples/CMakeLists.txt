add_executable(run_server
               "run_server.cc")

target_include_directories(run_server PRIVATE ${INC_DIR})
target_link_libraries(run_server PRIVATE ccmqsrv)

add_executable(run_client
               "run_client.cc")

target_include_directories(run_client PRIVATE ${INC_DIR})
target_link_libraries(run_client PRIVATE ccmqsrv)

add_executable(run_server_event
               "run_server_event.cc")

target_include_directories(run_server_event PRIVATE ${INC_DIR})
target_link_libraries(run_server_event PRIVATE ccmqsrv)

add_executable(run_client_event
               "run_client_event.cc")

target_include_directories(run_client_event PRIVATE ${INC_DIR})
target_link_libraries(run_client_event PRIVATE ccmqsrv)

add_executable(load_config
               "load_config.cc")

target_include_directories(load_config PRIVATE ${INC_DIR})
target_link_libraries(load_config PRIVATE ccmqsrv)