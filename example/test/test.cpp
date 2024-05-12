#include <any>
#include <iostream>
import viole_util;
int main() {
  int a = 1;
  std::cout << "a:" << a << std::endl;
  viole::any_point p(&a);
  p.set(5);
  std::cout << "a:" << a << std::endl;
  std::any o = 10;
  p.set_any(o);
  std::cout << "a:" << a << std::endl;
  int w = 7;
  std::cout << "w:" << w << std::endl;
  p.swap(&w);
  p.set(11);
  std::cout << "a:" << a << std::endl;
  std::cout << "w:" << w << std::endl;
  std::cout << "test" << std::endl;
  return 0;
}
