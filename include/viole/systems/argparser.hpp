#pragma once
#include "viole/base/base_type.hpp"
#include <string>
namespace viole {
/**
 * @class basic_arg
 * @brief [TODO:description]
 *
 */
class basic_arg : public base_object {
protected:
  std::string name, description;

public:
  virtual void parse(basic_input &input) = 0;
  virtual void information(basic_output &output) const noexcept;
  void to_string(basic_output &output) const noexcept override;
  [[nodiscard]] const std::type_info &get_type() const noexcept override;
  [[nodiscard]] std::string to_string() const noexcept override;
};
template <typename T> class basic_type_arg : public basic_arg {
protected:
  T *source;
  std::string arg_name;

public:
  void parse(basic_input &input) override = 0;
  basic_type_arg(std::string _name, std::string _description,
                 std::string arg_name, T *source) {
    name = _name;
    description = _description;
    this->source = source;
    this->arg_name = arg_name;
  }
  void information(basic_output &output) const noexcept override {
    output.write_char('[');
    output.write_string(arg_name);
    output.write_char(':');
    output.write_string(typeid(T).name());
    output.write_char(']');
  }
  [[nodiscard]] const std::type_info &get_type() const noexcept override {
    return typeid(basic_type_arg<T>);
  }
};
/**
 * @class args_parser
 * @brief [TODO:description]
 *
 */
class args_parser {
public:
  void parse(const char **v, int size);
};
} // namespace viole
