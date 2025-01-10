#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>
#include <amqpcpp.h>
#include "rpc_utils.h"
#include "conn_handler.h"

using namespace ccmqsrv;

int fib(int n)
{
    switch (n)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return fib(n - 1) + fib(n - 2);
    }
}

int foo(std::string arg)
{
    std::cout << arg << std::endl;
    return 0;
}

int
main(int argc, char* argv[])
{
    std::locale::global(std::locale("en_US.UTF-8"));
    ConnHandler handler;
    AMQP::TcpConnection connection(handler,
            AMQP::Address("localhost", 5672,
                          AMQP::Login("guest", "guest"), "/"));
    AMQP::TcpChannel channel(&connection);
    channel.onError([&handler](const char* message)
        {
            std::cout << "Channel error: " << message << std::endl;
            handler.Stop();
        });
    channel.setQos(1);
    // channel.declareExchange("mqsrv_rpc_exchange", AMQP::topic, AMQP::durable);
    // channel.declareQueue("test_rpc_queue", AMQP::exclusive+AMQP::autodelete);
    // channel.bindQueue("mqsrv_rpc_exchange", "test_rpc_queue", "test_rpc_queue");
    // channel.consume("test_rpc_queue", AMQP::noack)
    channel.declareQueue("rpc_queue");
    channel.consume("", AMQP::noack)
        .onReceived
        (
            [&channel](const AMQP::Message& m, uint64_t tag, bool)
            {
                std::cout << "req id: " << m.correlationID() << std::endl;
                std::cout << "reply-to: "<< m.replyTo() << std::endl;

                // std::cout << split_req(m.body()) << std::endl;
                std::string_view bd = m.body();
                std::cout << "body: " << bd.substr(0, m.bodySize()) << std::endl;
                // auto json_str = R"({[0, "corr-cb8bb95e-5422-448d-8a25-985c84793c43", "foo", ["test"], {}]})";
                // auto obj = json_loadb(json_str);
                // std::cout << *obj["name"].as_string() << std::endl;
                // auto body = m.body();
                // auto obj = json_loadb(body);

                AMQP::Envelope env(std::to_string(fib(std::stoi(m.body()))));
                env.setCorrelationID(m.correlationID());

                channel.publish("", m.replyTo(), env);
                std::cout << "tag:" << tag << std::endl;
                // channel.ack(tag);
            }
        );
    handler.Start();
    std::cout << "Closing connection." << std::endl;
    connection.close();
    return 0;
}
