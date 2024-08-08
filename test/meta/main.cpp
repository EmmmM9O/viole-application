#include "viole/meta.hpp"
#include <iostream>
#include <string>

DEFINE_STRUCT(test_meta_obj, (int)p1, (int)p2, (double)p3)
test_meta_obj bas = {5, 6, 0.3};
struct test_type_meta{
	int a1,a2;
	std::string test;
};
auto main() -> int {
  std::cout
      << viole::type_meta_to_string<test_type_meta, viole::default_type_pack>();
  std::cout << viole::meta_obj_to_string(bas);

  return 0;
}
