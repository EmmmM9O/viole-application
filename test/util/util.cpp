#include "viole/util.hpp"
#include "viole/base.hpp"
#include <iostream>

int main() {
  std::cout << "util test\n to_string:";
  viole::basic_object obj;
  std::cout << obj << ":" << viole::to_string(obj) << ":"
            << viole::to_string(123);
  return 0;
}
