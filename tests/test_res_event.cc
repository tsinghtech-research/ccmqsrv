#include <iostream>
#include <future>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <tuple>
#include <chrono>

class AsyncResult {
public:
  // 设置异步操作的结果
  template<typename... Args>
  void set(std::tuple<Args...> &&result) {
    std::unique_lock<std::mutex> lock(mutex_);
    result_ = std::move(result);
    done_ = true;
    cond_var_.notify_all();
  }

  template<typename... Args>
  void set(std::tuple<Args...> &result) {
    set(std::move(result));
  }

  // 获取异步操作的结果
  std::tuple<std::string, std::string>&& get(int timeout=1000) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!done_) {
      auto cv_status = cond_var_.wait_for(lock, std::chrono::milliseconds(timeout));
      if (cv_status == std::cv_status::timeout)
        throw std::runtime_error("call function timeout.");
    }
    done_ = false;
    return std::move(result_);
  }

  // 等待异步操作完成
  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this] { return done_; });
  }

private:
  std::mutex mutex_;
  std::condition_variable cond_var_;
  bool done_ = false;

  std::tuple<std::string, std::string> result_;
};

int main() {
    // 创建一个 AsyncResult 实例
    AsyncResult result;

    // 在一个线程中执行异步操作
    std::thread t([&result]() {
        // 模拟异步操作，例如从数据库获取数据
        std::this_thread::sleep_for(std::chrono::seconds(2));
        // 设置结果
        auto t = std::make_tuple("123", "hello");
        result.set(t);
    });
    // 在主线程中等待结果
    std::cout << "Waiting for the result..." << std::endl;
    auto value = result.get(300);
    std::cout << "Result: " << std::get<0>(value) << std::endl;

    // 等待线程结束
    t.join();

    return 0;
}