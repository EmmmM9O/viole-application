#include "viole/stl.hpp"
#include <iostream>
auto main() -> int {
  std::cout << "test stl" << std::endl;
  viole::queue<int> lis{};
  std::cout << lis << ":"<<std::endl;
  viole::queue<int> lis2{1, 2, 3};
  std::cout << ":" << lis2.pop()<<std::endl;
  std::cout << ":" << lis2.pop()<<std::endl;
  lis.push(4);
  std::cout << ":" << lis.pop()<<std::endl;
  return 0;
}
