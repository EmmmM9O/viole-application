#include "viole/mate.hpp"
#include <cxxabi.h>
#include <iostream>
#include <string>

struct test_obj {
  int te1, te2, te3;
  std::string str;
  double test2;
};
DEFINE_STRUCT(test_mate_obj, (int)p1, (int)p2)
test_mate_obj bas = {5, 6};
auto main() -> int {

  std::cout << "test mate" << std::endl;

  viole::type_mate_builder<test_obj, viole::type_builder<int>,
                           viole::type_builder<std::string>,
                           viole::type_builder<double>>
      mate_builder = {{{}, typeid(test_obj)}};
  auto mate = mate_builder.get();
  std::cout << "test_obj[";
  for (const auto &type : mate.contexts) {
    std::cout << abi::__cxa_demangle(type.type.name(), nullptr, nullptr,
                                     nullptr)
              << ",";
  }
  std::cout << "]" << std::endl << viole::mate_obj_to_string(bas);

  return 0;
}
