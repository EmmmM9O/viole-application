#include "viole/coroutines.hpp"
#include <future>
#include <thread>
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

auto async_executor::get_operator() -> executor_operator & {
  return m_operator;
}
auto async_executor::get_operator_point() -> executor_operator * {

  return &m_operator;
}
[[nodiscard]] auto
async_executor ::to_string() const noexcept -> viole::string {
  return "{async}";
}
[[nodiscard]] auto
async_executor::get_type() const noexcept -> const std::type_info & {
  return typeid(async_executor);
}
auto async_executor::execute(std::function<void()> &&func) -> void {
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

} // namespace viole
