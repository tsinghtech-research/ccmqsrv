#include <iostream>
class MessageQueueClient;

class Publisher {
public:
  Publisher(MessageQueueClient *client): client_(client) {}
  ~Publisher() {}

  void publish(const std::string& message);

private:
  MessageQueueClient *client_;
};

class MessageQueueClient {
public:
  MessageQueueClient() {}
  ~MessageQueueClient() {}

  // Publisher get_pubber() {return Publisher(this);}

  void pub() {}
};

void Publisher::publish(const std::string& message) {
  std::cout << "Publisher: " << message << std::endl;
  client_->pub();
}

int main() {
  MessageQueueClient client;
  return 0;
}