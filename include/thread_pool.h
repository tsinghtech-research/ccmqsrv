#pragma once
#include <queue>
#include <atomic>
#include <future>
#include <thread>
#include <functional>
#include <stdexcept>
 
class ThreadPool
{
using Task = std::function<void()>;
public:
  ThreadPool(int size = std::thread::hardware_concurrency() / 2) 
  {
    create_tasks(size);
  }

  ~ThreadPool()
  {
    is_run = false;
    condition.notify_all();
    for (std::thread& thread : container) 
    {
      if (thread.joinable())
      {
        thread.join();
      }
    }
  }

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
 
  template<class F, class... Args>
  auto start(F&& f, Args&&... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))> 
  {
    using ReturnType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    {
      std::unique_lock<std::mutex> lock{task_lock};
      if (!is_run) {
        exit(1);
      }

      all_tasks.emplace([task] { (*task)(); });
    }

    condition.notify_one();
    return task->get_future();
  }
  
private:
  void create_tasks(unsigned int size)
  {
    for (; size > 0; --size)
    {
      container.emplace_back([this]{
        while (true)
        {
          Task task;
          {
            std::unique_lock<std::mutex> lock{ task_lock };
            condition.wait(lock, [this]{return !is_run || !all_tasks.empty();});
            if (!is_run && all_tasks.empty())
            {
              return;
            }
            task = std::move(all_tasks.front()); 
            all_tasks.pop();
          }
          task();
        }
      });
    }
  }
  
  std::vector<std::thread> container;
  std::mutex task_lock;
  std::condition_variable condition;
  std::atomic<bool> is_run{true};
  std::queue<Task> all_tasks;
};