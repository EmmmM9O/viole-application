#include "viole/tuple.hpp"
#include "viole/util.hpp"
#include <iostream>
struct foo {
  int a, b;
};
auto main() -> int {
  std::cout << "test tuple" << std::endl;
  using tmp_pack = viole::type_pack<std::string, int, int>;
  using tuple = viole::pack_tuple<tmp_pack>::type;
  std::cout << viole::abi_type_to_string<tuple>() << std::endl;
  std::cout << viole::type_to_string<tuple>() << std::endl;

  std::cout << "------" << std::endl;
  std::cout << viole::pack_constructable<
                   foo, viole::type_pack<int, int, std::string>>::value
            << std::endl;
  std::cout << viole::pack_constructable<foo, viole::type_pack<int, int>>::value
            << std::endl;
  std::cout << "------" << std::endl;
  std::cout
      << viole::type_to_string<
             viole::combine_pack<viole::type_pack<int, double>,
                                 viole::type_pack<std::string, foo>>::type>()
      << std::endl;
  return 0;
}
