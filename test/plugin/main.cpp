#include <iostream>
class foo {
  [[viole::must_override]] virtual void test1();
  [[viole::must_override]] virtual void test2();
};
class bar : public foo {
  [[viole::must_override_del]] void test1() override;
  void test2() override;
};
class bar2 : public bar {

  void test2() override;
};
auto main() -> int {

  std::cout << "test plugin" << std::endl;
  return 0;
}
