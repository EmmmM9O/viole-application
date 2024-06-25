#include "viole/parser.hpp"
#include <iostream>
constexpr auto sign = viole::option(viole::one_of("+-"), '+');
constexpr auto number = viole::at_least(
    viole::one_of("1234567890"), 0,
    [](int acc, char chara) -> int { return acc * 10 + (chara - '0'); });
constexpr auto unsigned_int = viole::option(number, 0);
constexpr auto int_parser =
    viole::combine(sign, unsigned_int, [](char sign, int num) -> int {
      return sign == '+' ? num : -num;
    });
auto main() -> int {
  auto value = int_parser("123456");
  if (value.has_value()) {
    std::cout << value.value().first << std::endl;
  } else {
    std::cout << "parse error" << std::endl;
  }
  return 0;
}
