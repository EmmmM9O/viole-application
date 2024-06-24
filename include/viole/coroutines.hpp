#pragma once
#include "viole/base.hpp"
#include "viole/templates.hpp"
#include <concepts>
#include <coroutine>
#include <exception>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <type_traits>
namespace viole {

enum class task_statues { result, error, running };
class basic_executor : public basic_object {
public:
  /*
   * typename executor_operator
   * executor_operator &get_operator();;
   * executor_operator  *get_operator();;
   */
  basic_executor() = default;
  [[nodiscard]] auto to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
  virtual auto execute(std::function<void()> &&func) -> void = 0;
};
template <typename T>
concept basic_executor_template =
    std::is_base_of<basic_executor, T>::value && requires(T executor) {
      typename T::executor_operator;
      {
        executor.get_operator()
      } -> std::same_as<typename T::executor_operator &>;
      {
        executor.get_operator_point()
      } -> std::same_as<typename T::executor_operator *>;
      { T() };
    };
class noop_executor : public basic_executor {
public:
  noop_executor() = default;
  struct executor_operator {};
  auto get_operator() -> executor_operator &;
  auto get_operator_point() -> executor_operator *;
  [[nodiscard]] auto to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
  auto execute(std::function<void()> &&func) -> void override;

private:
  executor_operator m_operator;
};
TEMPLATE_FIT(noop_executor, basic_executor_template);
class async_executor : public basic_executor {
public:
  async_executor() = default;
  struct executor_operator {};
  auto get_operator() -> executor_operator &;
  auto get_operator_point() -> executor_operator *;
  [[nodiscard]] auto to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
  auto execute(std::function<void()> &&func) -> void override;

private:
  executor_operator m_operator;
};
TEMPLATE_FIT(async_executor, basic_executor_template);
class thread_executor : public basic_executor {
public:
  thread_executor();
  struct executor_operator {};
  auto get_operator() -> executor_operator &;
  auto get_operator_point() -> executor_operator *;
  [[nodiscard]] auto to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
  auto execute(std::function<void()> &&func) -> void override;

private:
  executor_operator m_operator;
};
TEMPLATE_FIT(thread_executor, basic_executor_template);
/*
class looper_executor : public basic_executor {
public:
  struct executor_operator {

  };
  auto get_operator() -> executor_operator &;
  auto get_operator_point() -> executor_operator *;
  [[nodiscard]] auto to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
  auto execute(std::function<void()> &&func) -> void override;
  looper_executor();
  ~looper_executor() override;
  auto shutdown(bool wait_for_complete = true) -> bool;
  auto join() -> void;

private:
  auto run_loop() -> void;
  std::condition_variable m_queue_condition;
  std::mutex m_queue_lock;
  std::queue<std::function<void()>> m_executable_queue;
  std::atomic<bool> m_is_active;
  std::thread m_work_thread;
  executor_operator m_operator;
};
TEMPLATE_FIT(looper_executor, basic_executor_template);*/
template <basic_executor_template Executor, typename Result,
          typename Error = std::exception_ptr>
class basic_task : public basic_object {
public:
  [[nodiscard]] auto to_string() const noexcept -> viole::string override {
    return string{"{"} + (done() ? "finished" : "running") + "}";
  }
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override {
    return typeid(basic_task<Executor, Result, Error>);
  }
  class task_awaiter {
  public:
    explicit task_awaiter(basic_executor *executor,
                          basic_task &&o_task) noexcept
        : m_executor(executor), m_task(std::move(o_task)) {}

    task_awaiter(task_awaiter &&completion) noexcept
        : basic_task(std::exchange(completion.m_executor, nullptr),
                     std::exchange(completion.task, {})) {}

    task_awaiter(task_awaiter &) = delete;

    auto operator=(task_awaiter &) -> task_awaiter & = delete;

    [[nodiscard]] constexpr auto await_ready() const noexcept -> bool {
      return false;
    }
    auto await_suspend(std::coroutine_handle<> handle) -> void {
      m_task.finally([handle, this]() {
        m_executor->execute([handle]() { handle.resume(); });
      });
    }
    auto await_resume() noexcept -> Result { return m_task.get_result(); }

  private:
    basic_task m_task;
    basic_executor *m_executor;
  };
  class dispatch_awaiter {
  public:
    explicit dispatch_awaiter(basic_executor *executor) noexcept
        : m_executor(executor) {}
    [[nodiscard]] auto await_ready() const -> bool { return false; }
    auto await_suspend(std::coroutine_handle<> handle) -> void {
      m_executor->execute([handle]() { handle.resume(); });
    }
    auto await_resume() -> void {}

  private:
    basic_executor *m_executor;
  };
  class task_result {
  public:
    explicit task_result() : m_error(std::nullopt), m_result(std::nullopt) {};

    explicit task_result(Result &&value)
        : m_result(value), m_error(std::nullopt) {}
    explicit task_result(Error &&error)
        : m_result(std::nullopt), m_error(error) {}
    auto get() -> Result {
      if (m_error) {
        std::rethrow_exception(m_error.value());
      }
      return m_result.value();
    }
    auto get_result() -> Result { return m_result.value(); }
    auto get_error() -> Error { return m_error.value(); }
    [[nodiscard]] auto statue() const -> task_statues {
      if (m_result.has_value()) {
        return task_statues::result;
      }
      if (m_error.has_value()) {
        return task_statues::error;
      }
      return task_statues::running;
    }

    [[nodiscard]] auto has_result() const -> bool {
      return m_result.has_value();
    }
    [[nodiscard]] auto has_error() const -> bool { return m_error.has_value(); }
    [[nodiscard]] auto finished() const -> bool {
      return m_result.has_value() || m_error.has_value();
    }

  private:
    std::optional<Result> m_result;
    std::optional<Error> m_error;
  };
  class promise_type {
  public:
    auto initial_suspend() -> dispatch_awaiter {
      return dispatch_awaiter{&m_executor};
    }
    auto final_suspend() noexcept -> std::suspend_always { return {}; }
    auto get_return_object() -> basic_task {
      return basic_task{
          &m_executor, std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    auto unhandled_exception() -> void {
      std::lock_guard lock(m_completion_lock);
      m_result = task_result{std::current_exception()};
      m_completion.notify_all();
      notify_callbacks();
    }
    auto return_value(Result value) -> void {
      std::lock_guard lock(m_completion_lock);
      m_result = task_result{std::move(value)};
      m_completion.notify_all();
      notify_callbacks();
    }
    template <typename other_result, typename other_error>
    auto await_transform(basic_task<other_result, other_error> &&o_task)
        -> basic_task<other_result, other_error>::task_awaiter {
      return basic_task<other_result, other_error>::task_awaiter(
          &m_executor, std::move(o_task));
    }
    auto get_result() -> Result {
      std::unique_lock lock(m_completion_lock);
      if (!m_result.has_value()) {
        m_completion.wait(lock);
      }
      return m_result->get();
    }
    auto on_completed(std::function<void(task_result)> &&func) -> void {
      std::unique_lock lock(m_completion_lock);
      if (m_result.has_value()) {
        auto value = m_result.value();
        lock.unlock();
        func(value);
      } else {
        m_completion_callbacks.push_back(func);
      }
    }

  private:
    Executor m_executor;
    std::list<std::function<void(task_result)>> m_completion_callbacks;
    void notify_callbacks() {
      auto value = m_result.value();
      for (auto &callback : m_completion_callbacks) {
        callback(value);
      }
      m_completion_callbacks.clear();
    }
    std::mutex m_completion_lock;
    std::condition_variable m_completion;
    std::optional<task_result> m_result;
  };
  auto operator=(basic_task &&o_task) noexcept -> basic_task & {
    m_handle = o_task.m_handle;
    return *this;
  }
  explicit basic_task(Executor *executor,
                      std::coroutine_handle<promise_type> handle)
      : m_handle(handle), m_executor(executor) {}

  [[nodiscard]] auto done() const -> bool {
    if (!m_handle) {
      return false;
    }
    return m_handle.done();
  }
  auto get_executor_operator() -> Executor::executor_operator & {
    return m_executor->get_operator();
  }
  auto operator->() const -> Executor::executor_operator * {
    return m_executor->get_operator_point();
  }
  auto get_result() -> Result { return m_handle.promise().get_result(); }
  basic_task(basic_task &&o_task) noexcept
      : m_handle(std::exchange(o_task.m_handle, {})) {}

  basic_task(basic_task &) = delete;

  auto operator=(basic_task &) -> basic_task & = delete;

  ~basic_task() override {
    if (m_handle) {
      m_handle.destroy();
    }
  }
  auto then(std::function<void(Result)> &&func) -> basic_task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_result()) {
        func(result.get_result());
      }
    });
    return *this;
  }
  auto catching(std::function<void(Error)> &&func) -> basic_task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_error()) {
        func(result.get_error());
      }
    });
    return *this;
  }
  auto finally(std::function<void()> &&func) -> basic_task & {
    m_handle.promise().on_completed([func](auto /*result*/) { func(); });
    return *this;
  }

private:
  std::coroutine_handle<promise_type> m_handle;
  Executor *m_executor;
};
template <basic_executor_template Executor, typename Error>
class basic_task<Executor, void, Error> : public basic_object {
public:
  [[nodiscard]] auto to_string() const noexcept -> viole::string override {
    return string{"{"} + (done() ? "finished" : "running") + "}";
  }
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override {
    return typeid(basic_task<Executor, void, Error>);
  }
  class task_awaiter {
  public:
    explicit task_awaiter(basic_executor *executor,
                          basic_task &&o_task) noexcept
        : m_task(std::move(o_task)), m_executor(executor) {}

    task_awaiter(task_awaiter &&completion) noexcept
        : basic_task(std::exchange(completion.m_executor, nullptr),
                     std::exchange(completion.task, {})) {}

    task_awaiter(task_awaiter &) = delete;

    auto operator=(task_awaiter &) -> task_awaiter & = delete;

    [[nodiscard]] constexpr auto await_ready() const noexcept -> bool {
      return false;
    }
    auto await_suspend(std::coroutine_handle<> handle) -> void {
      m_task.finally([handle, this]() {
        m_executor->execute([handle]() { handle.resume(); });
      });
    }
    auto await_resume() noexcept -> void { m_task.get_result(); }

  private:
    basic_task m_task;
    basic_executor *m_executor;
  };
  class dispatch_awaiter {
  public:
    explicit dispatch_awaiter(basic_executor *executor) noexcept
        : m_executor(executor) {}
    [[nodiscard]] auto await_ready() const -> bool { return false; }
    auto await_suspend(std::coroutine_handle<> handle) -> void {
      m_executor->execute([handle]() { handle.resume(); });
    }
    auto await_resume() -> void {}

  private:
    basic_executor *m_executor;
  };
  class task_result {
  public:
    explicit task_result() : m_error(std::nullopt) {};

    explicit task_result(bool /*void_run*/)
        : m_result(true), m_error(std::nullopt) {}
    explicit task_result(Error &&error) : m_error(error) {}
    auto get() -> void {
      if (m_error) {
        std::rethrow_exception(m_error.value());
      }
    }
    auto get_error() -> Error { return m_error; }
    [[nodiscard]] auto statue() const -> task_statues {
      if (m_result) {
        return task_statues::result;
      }
      if (m_error.has_value()) {
        return task_statues::error;
      }
      return task_statues::running;
    }

    [[nodiscard]] auto has_result() const -> bool { return m_result; }
    [[nodiscard]] auto has_error() const -> bool { return m_error.has_value(); }
    [[nodiscard]] auto finished() const -> bool {
      return m_result || m_error.has_value();
    }

  private:
    bool m_result = false;
    std::optional<Error> m_error;
  };
  class promise_type {
  public:
    auto initial_suspend() -> dispatch_awaiter {
      return dispatch_awaiter{&m_executor};
    }
    auto final_suspend() noexcept -> std::suspend_always { return {}; }
    auto get_return_object() -> basic_task {
      return basic_task{
          &m_executor,
          std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    auto unhandled_exception() -> void {
      std::lock_guard lock(m_completion_lock);
      m_result = task_result{std::current_exception()};
      m_completion.notify_all();
      notify_callbacks();
    }
    auto return_void() -> void {
      std::lock_guard lock(m_completion_lock);
      m_result = task_result{true};
      m_completion.notify_all();
      notify_callbacks();
    }
    template <typename other_result, typename other_error>
    auto await_transform(basic_task<other_result, other_error> &&o_task)
        -> basic_task<other_result, other_error>::task_awaiter {
      return typename basic_task<other_result, other_error>::task_awaiter(
          &m_executor, std::move(o_task));
    }
    auto get_result() -> void {
      std::unique_lock lock(m_completion_lock);
      if (!m_result.has_value()) {
        m_completion.wait(lock);
      }
      return m_result->get();
    }
    auto on_completed(std::function<void(task_result)> &&func) -> void {
      std::unique_lock lock(m_completion_lock);
      if (m_result.has_value()) {
        auto value = m_result.value();
        lock.unlock();
        func(value);
      } else {
        m_completion_callbacks.push_back(func);
      }
    }

  private:
    Executor m_executor;
    std::list<std::function<void(task_result)>> m_completion_callbacks;
    void notify_callbacks() {
      auto value = m_result.value();
      for (auto &callback : m_completion_callbacks) {
        callback(value);
      }
      m_completion_callbacks.clear();
    }
    std::mutex m_completion_lock;
    std::condition_variable m_completion;
    std::optional<task_result> m_result;
  };
  auto operator=(basic_task &&o_task) noexcept -> basic_task & {
    m_handle = o_task.m_handle;
    return *this;
  }
  explicit basic_task(Executor *executor,
                      std::coroutine_handle<promise_type> handle)
      : m_executor(executor), m_handle(handle) {}

  [[nodiscard]] auto done() const -> bool {
    if (!m_handle) {
      return false;
    }
    return m_handle.done();
  }
  auto get_result() -> void { return m_handle.promise().get_result(); }
  basic_task(basic_task &&o_task) noexcept
      : m_handle(std::exchange(o_task.m_handle, {})) {}

  basic_task(basic_task &) = delete;

  auto operator=(basic_task &) -> basic_task & = delete;
  auto get_executor_operator() -> Executor::executor_operator & {
    return m_executor->get_operator();
  }
  auto operator->() const -> Executor::executor_operator * {
    return m_executor->get_operator_point();
  }
  ~basic_task() override {
    if (m_handle) {
      m_handle.destroy();
    }
  }
  auto then(std::function<void()> &&func) -> basic_task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_result()) {
        func();
      }
    });
    return *this;
  }
  auto catching(std::function<void(Error)> &&func) -> basic_task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_error()) {
        func(result.get_error());
      }
    });
    return *this;
  }
  auto finally(std::function<void()> &&func) -> basic_task & {
    m_handle.promise().on_completed([func](auto /*result*/) { func(); });
    return *this;
  }

private:
  std::coroutine_handle<promise_type> m_handle;
  Executor *m_executor;
};
template <typename T>
using noop_task = basic_task<noop_executor, T, std::exception_ptr>;
using noop_runnable_task = basic_task<noop_executor, void, std::exception_ptr>;
} // namespace viole
