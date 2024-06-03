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
  [[nodiscard]] virtual std::string information() const noexcept;
  void to_string(basic_output &output) const noexcept override;
  [[nodiscard]] const std::type_info &get_type() const noexcept override;
  [[nodiscard]] std::string to_string() const noexcept override;
};
template <typename T> class basic_type_arg : public basic_arg {
protected:
  T *source;

public:
  void parse(basic_input &input) override;
  basic_type_arg(std::string _name, std::string _description, T *source) {
    name = _name;
    description = _description;
    this->source = source;
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
