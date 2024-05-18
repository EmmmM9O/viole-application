#pragma once
#include <string>
#include <typeinfo>
namespace viole {
class basic_output {
public:
  virtual void write_string(const std::string &str) = 0;
  virtual void write_int(int value) = 0;
  virtual void write_char(char value) = 0;
};
class basic_input {
public:
  virtual std::string read_string() = 0;
  virtual int read_int() = 0;
  virtual char read_char() = 0;
};
class base_object {
public:
  [[nodiscard]] virtual std::string to_string() const noexcept = 0;
  virtual void to_string(basic_output &output) const noexcept = 0;
  [[nodiscard]] virtual const std::type_info &get_type() const noexcept = 0;
};
} // namespace viole
