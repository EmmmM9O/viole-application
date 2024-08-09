#include "viole/json.hpp"
#include "viole/meta.hpp"
#include <array>
#include <iostream>
#include <string>

DEFINE_STRUCT(test_meta_obj, (int)p1, (std::string)str, (std::array<int, 2>)arr)
test_meta_obj obj{4, "test", {1, 2}};
auto main() -> int {
  std::cout << "test_json\n";
  std::cout << viole::obj_to_json(obj, false);
  return 0;
}
