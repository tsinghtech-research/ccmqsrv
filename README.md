# CCMQSRV

## Introduction

CCMQSRV is a simple C++ RPC framework based on RabbitMQ, a lightweight messaging queue system. It provides a simple interface for clients to send and receive messages. The framework is designed to be easy to use and provides a simple way to implement RPC services. The original purpose of the framework was to implement RPC calls between c++ and python. 

## Features

- Simple interface for clients to send and receive messages.
- Easy to use and provides a simple way to implement RPC services.
- Supports multiple clients and servers.
- Support multithreadings.

## Limitations

- Passing python <k=v> parameters to c++ servers is not supported.
- The python clients must pass list arguments of the same type.
- The python client must pass dictionary arguments with the string key and the value of the same type.

## Getting Started

### Dependencies

- RabbitMQ (version 3.8.10 or later)
- cxx-compiler (gcc11 or later)
- CMake (version 3.14 or later)
- ninja
- spdlog(version <= 1.10.0>)
- nlohmann_json
- amqp-cpp
- libevent
- libuuid
- toml11
- thread-pool(https://github.com/bshoshany/thread-pool.git)

Create a new conda environment to compile the build.
Install the above third-party libraries.

### Building

To build the project, follow these steps:

1. Clone the repository:

```Bash
git clone https://github.com/tsinghtech-research/ccmqsrv.git
```

2. Create a build directory and change to it:

```Bash
mkdir build && cd build
```

3. Run build script:

```Bash
# activate conda env
conda activate xxx
# cmake build
sh ../build_install.sh
```

4. Build the project and install it:

```Bash
ninja install
```

### Running the Server and Client Examples

1. Run the server:

```Bash
# C++ server
./examples/run_server

# or Python server
python examples/run_server.py
```

2. Run the client:

```Bash
# C++ client
./examples/run_client

# or Python client
python examples/run_client.py
```

3. Run event server:

```Bash
# C++ event server
./examples/run_server_event

# or Python event server
python examples/run_server_event.py
```

4. Run event client:

```Bash
# C++ event client
./examples/run_client_event

# or Python event client  
python examples/run_client_event.py
```

## License

This project is licensed under the Apache License - see the [LICENSE](LICENSE) file for details. 
