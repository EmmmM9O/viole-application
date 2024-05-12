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
  p.emplace(&w);
  p.set(11);
  std::cout << "a:" << a << std::endl;
  std::cout << "w:" << w << std::endl;
  p=1145;
  std::cout << "a:" << a << std::endl;
  std::cout << "w:" << w << std::endl;
  viole::any_point p2(&a);
  p2 = (22);
  std::cout << "a:" << a << std::endl;
  std::cout << "w:" << w << std::endl;
  p = p2;
  p.set(14);
  std::cout << "a:" << a << std::endl;
  std::cout << "w:" << w << std::endl;
  p2.set(16);
  std::cout << "a:" << a << std::endl;
  std::cout << "w:" << w << std::endl;
  std::cout << "test" << std::endl;
  return 0;
}
