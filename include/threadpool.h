//
// Created by kaylor on 3/5/24.
//
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(size_t);
  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;
  ~ThreadPool();
  bool IsTasksEmpty();
  int TasksSize();

 private:
  // need to keep track of threads so we can join them
  std::vector<std::thread> workers;// 工作线程列表
  // the task queue
  std::queue<std::function<void()> > tasks;// 任务队列

  // synchronization
  std::mutex queue_mutex;// 任务队列互斥锁
  std::condition_variable condition;// 条件变量（用于阻塞线程）
  bool stop;// 线程池是否停止
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
  for (size_t i = 0; i < threads; ++i)
    workers.emplace_back([this](int i) {
      auto thread_name = "thread" + std::to_string(i);
      pthread_setname_np(pthread_self(), thread_name.c_str());
      for (;;) {
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(this->queue_mutex);
          this->condition.wait(
              lock, [this] { return this->stop || !this->tasks.empty(); });
          if (this->stop && this->tasks.empty()) return;
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }

        task();
      }
    }, i);
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()> >(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(queue_mutex);

    // don't allow enqueueing after stopping the pool
    if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

    tasks.emplace([task]() { (*task)(); });
  }
  condition.notify_one();
  return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  for (std::thread& worker : workers) worker.join();
}

inline bool ThreadPool::IsTasksEmpty() {
  std::lock_guard<std::mutex> lock_guard(queue_mutex);
  return tasks.empty();
}

inline int ThreadPool::TasksSize() {
  std::lock_guard<std::mutex> lock_guard(queue_mutex);
  return tasks.size();
}

#endif