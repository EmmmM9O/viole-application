#include "viole/filesystem.hpp"
#include <iostream>

int main() {
  std::cout << "test filesystem	";
  viole::fi test{"./test.log"};
  std::cout << "\npath:" << test.path();
  std::cout << "\nabsolute:" << test.absolute_path();
  try {
    test.touch();
    std::cout<<"\ntouch\n";
    test.write_string("test");
    std::cout << "\ncontext:" << test.read_string();
    test--;
    std::cout << "\nabsolute:" << test.absolute_path();
    test += "test2.log";
    std::cout << "\nabsolute:" << test.absolute_path();
  } catch (std::exception &exception) {
    std::cout << exception.what() << "\n";
  }
  return 0;
}
