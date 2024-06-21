#include "viole/base.hpp"
#include "viole/util.hpp"
#include <iostream>
enum class T1 { y1, y2 };
int main() {
  std::cout << "util test\n to_string:";
  viole::basic_object obj;
  T1 foo = T1::y1;
  std::cout << obj << ":" << viole::to_string(obj) << ":"
            << viole::to_string(foo) << ":" << viole::to_string(123) << ":"
            << viole::enum_to_index<T1>("y2") << ":"
            << viole::enum_to_string(static_cast<T1>(4)) << ":"
            << viole::enum_max_size<T1>();
  std::vector<viole::string> values;
  viole::enum_values<T1>(values);
  for (const auto &val : values) {
    std::cout << "value:" << val << ";";
  }
  std::cout << "\n" << viole::enum_class_to_string<T1>();
  return 0;
}
