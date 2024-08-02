#include "viole/coroutines.hpp"
#include "viole/util.hpp"
#include <atomic>
#include <coroutine>
#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <memory>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
namespace viole {

[[nodiscard]] auto basic_executor::to_string() const noexcept -> viole::string {
  return "{abstract class}";
}
[[nodiscard]] auto
basic_executor::get_type() const noexcept -> const std::type_info & {
  return typeid(basic_executor);
}

auto noop_executor::get_operator() -> executor_operator & { return m_operator; }
auto noop_executor::get_operator_point() -> executor_operator * {
  return &m_operator;
}
[[nodiscard]] auto noop_executor::to_string() const noexcept -> viole::string {
  return "{noop}";
}
[[nodiscard]] auto
noop_executor ::get_type() const noexcept -> const std::type_info & {
  return typeid(noop_executor);
}

auto noop_executor::execute(std::function<void()> &&func) -> void { func(); }

auto thread_async_executor::get_operator() -> executor_operator & {
  return m_operator;
}
auto thread_async_executor::get_operator_point() -> executor_operator * {

  return &m_operator;
}
[[nodiscard]] auto
thread_async_executor ::to_string() const noexcept -> viole::string {
  return "{async}";
}
[[nodiscard]] auto
thread_async_executor::get_type() const noexcept -> const std::type_info & {
  return typeid(thread_async_executor);
}
auto thread_async_executor::execute(std::function<void()> &&func) -> void {
  auto future = std::async(func);
}

auto thread_executor::get_operator() -> executor_operator & {
  return m_operator;
}
auto thread_executor::get_operator_point() -> executor_operator * {
  return &m_operator;
}
[[nodiscard]] auto
thread_executor::to_string() const noexcept -> viole::string {
  return "{thread}";
}
[[nodiscard]] auto
thread_executor::get_type() const noexcept -> const std::type_info & {
  return typeid(thread_executor);
}
auto thread_executor::execute(std::function<void()> &&func) -> void {
  std::thread(func).detach();
}
static auto get_thread_id_string(const std::thread::id &tid) -> string {
  std::stringstream sin;
  sin << tid;
  return sin.str();
}
// looper
[[nodiscard]] auto
basic_async_looper::to_string() const noexcept -> viole::string {
  return string{"{is_active:"} + (is_active() ? "true" : "false") +
         ",task_size:" + viole::to_string(task_size()) + "}";
}
[[nodiscard]] auto
basic_async_looper::get_type() const noexcept -> const std::type_info & {
  return typeid(basic_async_looper);
}
[[nodiscard]] auto st_async_looper::task_size() const noexcept -> size_t {
  return m_executable_queue.size();
}
[[nodiscard]] auto st_async_looper::is_active() const noexcept -> bool {
  return m_is_active.load(std::memory_order_relaxed);
}
auto st_async_looper::shutdown(bool wait_for_complete) -> void {
  m_is_active.store(false, std::memory_order_relaxed);
  if (!wait_for_complete) {
    std::unique_lock lock(m_queue_lock);
    decltype(m_executable_queue) empty_queue;
    std::swap(m_executable_queue, empty_queue);
    lock.unlock();
  }
  m_queue_condition.notify_all();
}
auto st_async_looper::start() -> void {
  m_is_active.store(true, std::memory_order_relaxed);
  m_work_thread = std::thread(&st_async_looper::run_loop, this);
}
st_async_looper::st_async_looper() {
  m_is_active.store(false, std::memory_order_relaxed);
}
st_async_looper::~st_async_looper() {
  shutdown(false);
  join();
}
[[nodiscard]] auto
st_async_looper::to_string() const noexcept -> viole::string {
  return "{task_size:" + viole::to_string(m_executable_queue.size()) +
         ",is_active:" +
         (m_is_active.load(std::memory_order_relaxed) ? "true" : "false") +
         +",thread:" + get_thread_id_string(m_work_thread.get_id()) + "}";
}
[[nodiscard]] auto
st_async_looper::get_type() const noexcept -> const std::type_info & {
  return typeid(st_async_looper);
}
auto st_async_looper::join() -> void {
  if (m_work_thread.joinable()) {
    m_work_thread.join();
  }
}
auto st_async_looper::execute(std::function<bool()> &&func) -> void {
  std::unique_lock lock(m_queue_lock);
  if (m_is_active.load(std::memory_order_relaxed)) {
    m_executable_queue.push(func);
    lock.unlock();
    m_queue_condition.notify_one();
  }
}

auto func_awaiter::get_awaiter() -> awaiter {
  return awaiter{std::move(m_func)};
}

func_awaiter::func_awaiter(std::function<void(std::coroutine_handle<>)> &&func)
    : m_func(func) {}
func_awaiter::awaiter::awaiter(
    std::function<void(std::coroutine_handle<>)> &&func)
    : m_func(func) {}

auto func_awaiter::awaiter::await_suspend(std::coroutine_handle<> handle)
    -> void {
  m_func(handle);
}
auto func_awaiter::awaiter::await_resume() noexcept -> void {}
/*
struct st_async_awaiter {
  explicit st_async_awaiter(st_async_looper *looper,
                            std::function<void()> &&func)
      : m_looper(looper), m_func(func) {}
  [[nodiscard]] constexpr auto await_ready() const noexcept -> bool {
    return false;
  }
  auto await_suspend(std::coroutine_handle<> handle) -> void {
    //    m_looper->execute([this]() {
    std::thread([this, handle]() {
      m_func();
      std::cout << "before resume\n";
      handle.resume();
      std::cout << "after resume\n";
    }).detach();
  }
  auto await_resume() noexcept -> void {}
  struct is_awaiter {};

private:
  st_async_looper *m_looper;
  std::function<void()> m_func;
};
*/
// Well it semms that std::coroutine_handle<>  can not work on other thread
[[nodiscard]] auto async_promise::to_string() const noexcept -> viole::string {
  return string("{promise of{") + m_looper->to_string_full() + "}}";
}

auto async_promise::set_index(size_t index) -> async_promise & {
  m_index = index;
  return *this;
}
auto async_promise::set_finish() -> async_promise & {
  m_finish.store(true, std::memory_order_relaxed);
  m_condition.notify_all();
  return *this;
}
[[nodiscard]] auto
async_promise::get_type() const noexcept -> const std::type_info & {
  return typeid(async_promise);
}
auto async_promise::post() -> async_promise & {
  m_looper->execute([this]() {
    auto res = m_func(std::move(m_then), this);
    return res;
  });
  m_post = true;
  return *this;
}
auto async_promise::then(function_type &&func) -> async_promise & {
  m_then = [func, this]() {
    func();
    m_looper->reset_promise(m_index);
  };
  return *this;
}
auto async_promise::work() -> async_promise & {
  if (!m_post) {
    post();
  }
  std::unique_lock lock(m_lock);
  m_condition.wait(
      lock, [this]() { return m_finish.load(std::memory_order_relaxed); });
  return *this;
}

[[nodiscard]] auto async_promise::finished() const -> bool {
  return m_finish.load(std::memory_order_relaxed);
}
async_promise::async_promise(
    basic_async_looper *looper,
    std::function<bool(function_type &&, async_promise *)> &&func)
    : m_looper(looper), m_func(func), m_index(0) {

  m_then = [this]() { m_looper->reset_promise(m_index); };
}
auto st_async_looper::reset_promise(size_t index) -> void {
  m_promises[index] = nullptr;
}
auto st_async_looper::execute_async(std::function<void()> &&func)
    -> std::shared_ptr<async_promise> {
  std::shared_ptr<async_promise> ptr = std::make_shared<async_promise>(
      this, [func](auto &&then, async_promise *pro) -> bool {
        func();
        pro->set_finish();
        if (then) {
          then();
        }
        return false;
      });
  m_promises.push_back(ptr);
  ptr->set_index(m_promises.size() - 1);
  return ptr;
}

auto st_async_looper::execute_util(std::function<bool()> &&func)
    -> std::shared_ptr<async_promise> {
  std::shared_ptr<async_promise> ptr = std::make_shared<async_promise>(
      this, [func](std::function<void()> &&then, async_promise *pro) -> bool {
        if (func()) {
          pro->set_finish();
          if (then) {
            then();
          }
          return false;
        }
        return true;
      });
  m_promises.push_back(ptr);
  ptr->set_index(m_promises.size() - 1);
  return ptr;
}

auto st_async_looper::run_loop_once() -> void {
  if (m_executable_queue.empty()) {
    return;
  }
  std::unique_lock lock(m_queue_lock);
  if (m_executable_queue.empty()) {
    m_queue_condition.wait(lock);
    if (m_executable_queue.empty()) {
      return;
    }
  }
  auto func = m_executable_queue.front();
  m_executable_queue.pop();
  lock.unlock();
  if (func()) {
    lock.lock();
    m_executable_queue.push(func);
    lock.unlock();
  }
}
auto st_async_looper::run_loop() -> void {
  while (m_is_active.load(std::memory_order_relaxed) ||
         !m_executable_queue.empty()) {
    std::unique_lock lock(m_queue_lock);
    if (m_executable_queue.empty()) {
      m_queue_condition.wait(lock);
      if (m_executable_queue.empty()) {
        return;
      }
    }
    auto func = m_executable_queue.front();
    m_executable_queue.pop();
    lock.unlock();
    if (func()) {
      lock.lock();
      m_executable_queue.push(func);
      lock.unlock();
    }
  }
}

looper_executor::executor_operator::executor_operator(looper_executor *executor)
    : m_executor(executor) {}
auto looper_executor::executor_operator::wrok_on(basic_async_looper *looper)
    -> executor_operator & {
  m_executor->m_looper = looper;
  return *this;
}
auto looper_executor::executor_operator::shutdown(bool wait_for_complete)
    -> executor_operator & {
  m_executor->m_looper->shutdown(wait_for_complete);
  return *this;
}
[[nodiscard]] auto
looper_executor::executor_operator::task_size() const noexcept -> size_t {
  return m_executor->m_looper->task_size();
}
[[nodiscard]] auto
looper_executor::executor_operator::is_active() const noexcept -> bool {
  return m_executor->m_looper->is_active();
}
auto looper_executor::executor_operator::execute(std::function<void()> &&func)
    -> executor_operator & {
  m_executor->execute(std::forward<std::function<void()>>(func));
  return *this;
}

auto looper_executor::get_operator() -> executor_operator & {
  return m_operator;
}
auto looper_executor::get_operator_point() -> executor_operator * {
  return &m_operator;
}
[[nodiscard]] auto
looper_executor::to_string() const noexcept -> viole::string {
  return "{looper:" + m_looper->to_string_full() + "}";
}
[[nodiscard]] auto
looper_executor::get_type() const noexcept -> const std::type_info & {
  return typeid(looper_executor);
}
auto looper_executor::execute(std::function<void()> &&func) -> void {
  if (m_looper == nullptr) {
    main_thread_looper
        .execute_util([this]() -> bool { return m_looper != nullptr; })
        ->then([this, func]() {
          m_looper->execute([func]() {
            func();
            return false;
          });
        })
        .post();
  } else {
    m_looper->execute([func]() {
      func();
      return false;
    });
  }
}
looper_executor::looper_executor() : m_operator(this), m_looper(nullptr) {}

st_async_looper main_thread_looper{};
} // namespace viole
