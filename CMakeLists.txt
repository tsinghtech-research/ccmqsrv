cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(
    ccmqsrv
    VERSION 0.0.1
    DESCRIPTION "ccmqsrv"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

set(INC_DIR
    "${PROJECT_SOURCE_DIR}/include"
    "${CMAKE_PREFIX_PATH}/include"
)

find_package(spdlog REQUIRED)
find_package(amqpcpp REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(toml11 REQUIRED)

add_subdirectory(src)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

install(TARGETS ccmqsrv
        EXPORT ccmqsrv_targets
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
