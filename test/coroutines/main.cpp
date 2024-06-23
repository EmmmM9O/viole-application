#include "viole/coroutines.hpp"
#include <exception>
#include <iostream>
#include <thread>
auto simple_task2() -> viole::task<int, std::exception_ptr> {
  std::cout << ("task 2 start ...\n");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);
  std::cout << ("task 2 returns after 1s.\n");
  co_return 2;
}
auto simple_task1() -> viole::task<int, std::exception_ptr> {
  std::cout << ("task 1 start ...\n");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(2s);
  std::cout << ("task 2 returns after 2s.\n");
  co_return 1;
}
auto simple_task() -> viole::task<void, std::exception_ptr> {
  std::cout << ("task start ...\n");
  auto result1 = co_await simple_task1();
  std::cout << ("task 1 returns ") << result1;
  auto result2 = co_await simple_task2();
  std::cout << ("task 2 returns ") << result2;
  co_return;
}
auto main() -> int {
  simple_task()
      .then([]() { std::cout << ("task then ...\n"); })
      .finally([]() { std::cout << ("task finish ...\n"); })
      .get_result();
  return 0;
}
