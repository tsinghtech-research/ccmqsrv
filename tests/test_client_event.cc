#include "client.h"

using namespace ccmqsrv;

int main() {
  auto client = make_client();
  auto pub = client.get_pubber("test_event_queue");

  for (int i = 0; i < 10; i++) {
    std::cout << "Sending event " << i << std::endl;
    pub("test_event", std::map<string, int>({{"1", i}, {"2", i+1}, {"3", i+2}}));
  }

  return 0;
}