#include <iostream>
#include <thread>
#include <future>

// 定义一个任务函数
void task() {
    // 执行一些工作
    std::cout << "Thread is doing work." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 工作完成
    std::cout << "Thread has completed its work." << std::endl;
}

int main() {
    // 使用 std::async 启动任务，它会立即返回一个 std::future 对象
    std::future<void> future = std::async(std::launch::async, task);

    // 等待异步操作完成
    future.wait();
    std::cout << "Async operation has completed." << std::endl;

    // 异步操作完成后，线程将自动被分离
    // 如果你想要显式地分离线程，可以使用 detach，但这通常不是必要的
    // future.get() 会阻塞直到线程完成，然后线程将自动被分离

    return 0;
}