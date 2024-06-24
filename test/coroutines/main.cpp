#include "viole/coroutines.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
auto simple_task2() -> viole::noop_task<int> {
  std::cout << ("task 2 start ...\n");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);
  std::cout << ("task 2 returns after 1s.\n");
  co_return 2;
}
auto simple_task1() -> viole::noop_task<int> {
  std::cout << ("task 1 start ...\n");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(2s);
  std::cout << ("task 2 returns after 2s.\n");
  co_return 1;
}
auto print(const std::string &str) -> void {
  //  std::lock_guard lock(mut);
  std::cout << str << std::endl;
}
auto simple_task() -> viole::noop_runnable_task {
  std::cout << ("task start ...\n");
  auto result1 = co_await simple_task1();
  std::cout << ("task 1 returns ") << result1;
  auto result2 = co_await simple_task2();
  std::cout << ("task 2 returns ") << result2;
}
auto mst() -> viole::async_task<void> {
  std::cout << ("mst start ...\n");
  auto res = co_await simple_task1();
  co_return;
}
std::mutex mut;

auto main() -> int {

  viole::main_thread_looper.start();

  simple_task()
      .then([]() { print("task then ..."); })
      .finally([]() { print("task finish ..."); })
      .get_result();
  viole::main_thread_looper
      .execute_async([]() { print("async exector start"); })
      ->then([]() { print("async exector finish"); })
      .post()
      .work();
  int size = 0;
  auto k = viole::main_thread_looper
               .execute_util([&size]() {
                 if (size >= 10) {
                   print("util finish");
                   return true;
                 }
                 print("start");
                 size++;
                 return false;
               })
               ->then([]() { print("async exector util finish"); })
               .post()
               .work();
  auto prom = mst();
  prom->wrok_on(&viole::main_thread_looper);
  prom.then([]() { print("mst end"); }).get_result();

  viole::main_thread_looper.shutdown(true);
  viole::main_thread_looper.join();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  return 0;
}
