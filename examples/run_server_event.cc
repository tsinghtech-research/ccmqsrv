#include "server.h"

using namespace ccmqsrv;

void event(const std::string& evt_type, std::map<string, int> evt_data) {
  spdlog::info("Received event: {} {}", evt_type, evt_data.size());
  for (auto& [key, value] : evt_data) {
    spdlog::info("{}: {}", key, value); 
  }
}

int main() {
  spdlog::set_level(spdlog::level::debug);
  auto server = make_server("", "", "test_event_queue");
  server.register_event_handler("test_event", event);
  run_server(server);
  return 0;
}