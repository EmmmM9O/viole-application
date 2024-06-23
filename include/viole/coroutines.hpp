#pragma once
#include "viole/base.hpp"
#include <coroutine>
#include <exception>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
namespace viole {

enum class task_statues { result, error, running };
template <typename Result, typename Error> class task : public basic_object {
public:
  [[nodiscard]] auto to_string() const noexcept -> viole::string override {
    return string{"{"} + (done() ? "finished" : "running") + "}";
  }
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override {
    return typeid(task<Result, Error>);
  }
  class task_awaiter {
  public:
    explicit task_awaiter(task &&o_task) noexcept : m_task(std::move(o_task)) {}

    task_awaiter(task_awaiter &&completion) noexcept
        : task(std::exchange(completion.task, {})) {}

    task_awaiter(task_awaiter &) = delete;

    auto operator=(task_awaiter &) -> task_awaiter & = delete;

    [[nodiscard]] constexpr auto await_ready() const noexcept -> bool {
      return false;
    }
    auto await_suspend(std::coroutine_handle<> handle) -> void {
      m_task.finally([handle]() { handle.resume(); });
    }
    auto await_resume() noexcept -> Result { return m_task.get_result(); }

  private:
    task m_task;
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
    auto initial_suspend() -> std::suspend_never { return {}; }
    auto final_suspend() noexcept -> std::suspend_always { return {}; }
    auto get_return_object() -> task {
      return task{std::coroutine_handle<promise_type>::from_promise(*this)};
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
    auto await_transform(task<other_result, other_error> &&o_task)
        -> task<other_result, other_error>::task_awaiter {
      return task<other_result, other_error>::task_awaiter(std::move(o_task));
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
  explicit task(std::coroutine_handle<promise_type> handle)
      : m_handle(handle) {}

  auto done() const -> bool {
    if (!m_handle) {
      return false;
    }
    return m_handle.done();
  }
  auto get_result() -> Result { return m_handle.promise().get_result(); }
  task(task &&o_task) noexcept : m_handle(std::exchange(o_task.m_handle, {})) {}

  task(task &) = delete;

  auto operator=(task &) -> task & = delete;

  ~task() override {
    if (m_handle) {
      m_handle.destroy();
    }
  }
  auto then(std::function<void(Result)> &&func) -> task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_result()) {
        func(result.get_result());
      }
    });
    return *this;
  }
  auto catching(std::function<void(Error)> &&func) -> task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_error()) {
        func(result.get_error());
      }
    });
    return *this;
  }
  auto finally(std::function<void()> &&func) -> task & {
    m_handle.promise().on_completed([func](auto /*result*/) { func(); });
    return *this;
  }

private:
  std::coroutine_handle<promise_type> m_handle;
};
template <typename Error> class task<void, Error> : public basic_object {
public:
  [[nodiscard]] auto to_string() const noexcept -> viole::string override {
    return string{"{"} + (done() ? "finished" : "running") + "}";
  }
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override {
    return typeid(task<void, Error>);
  }
  class task_awaiter {
  public:
    explicit task_awaiter(task &&o_task) noexcept : m_task(std::move(o_task)) {}

    task_awaiter(task_awaiter &&completion) noexcept
        : task(std::exchange(completion.task, {})) {}

    task_awaiter(task_awaiter &) = delete;

    auto operator=(task_awaiter &) -> task_awaiter & = delete;

    [[nodiscard]] constexpr auto await_ready() const noexcept -> bool {
      return false;
    }
    auto await_suspend(std::coroutine_handle<> handle) -> void {
      m_task.finally([handle]() { handle.resume(); });
    }
    auto await_resume() noexcept -> void { m_task.get_result(); }

  private:
    task m_task;
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
    auto initial_suspend() -> std::suspend_never { return {}; }
    auto final_suspend() noexcept -> std::suspend_always { return {}; }
    auto get_return_object() -> task {
      return task{std::coroutine_handle<promise_type>::from_promise(*this)};
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
    auto await_transform(task<other_result, other_error> &&o_task)
        -> task<other_result, other_error>::task_awaiter {
      return typename task<other_result, other_error>::task_awaiter(std::move(o_task));
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
  explicit task(std::coroutine_handle<promise_type> handle)
      : m_handle(handle) {}

  auto done() const-> bool {
    if (!m_handle) {
      return false;
    }
    return m_handle.done();
  }
  auto get_result() -> void { return m_handle.promise().get_result(); }
  task(task &&o_task) noexcept : m_handle(std::exchange(o_task.m_handle, {})) {}

  task(task &) = delete;

  auto operator=(task &) -> task & = delete;

  ~task() override {
    if (m_handle) {
      m_handle.destroy();
    }
  }
  auto then(std::function<void()> &&func) -> task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_result()) {
        func();
      }
    });
    return *this;
  }
  auto catching(std::function<void(Error)> &&func) -> task & {
    m_handle.promise().on_completed([func](auto result) {
      if (result.has_error()) {
        func(result.get_error());
      }
    });
    return *this;
  }
  auto finally(std::function<void()> &&func) -> task & {
    m_handle.promise().on_completed([func](auto /*result*/) { func(); });
    return *this;
  }

private:
  std::coroutine_handle<promise_type> m_handle;
};
} // namespace viole
