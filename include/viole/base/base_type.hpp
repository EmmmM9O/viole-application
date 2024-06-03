/**
 * @file
 * @brief basic types
 * @author novarc
 * @date 2024-5-19
 */

#pragma once
#include <string>
#include <typeinfo>
namespace viole {
/**
 * @class basic_output
 * @brief [TODO:description]
 *
 */
class basic_output {
public:
  /**
   * @brief [TODO:description]
   *
   * @param str [TODO:parameter]
   */
  virtual void write_string(const std::string &str) = 0;
  /**
   * @brief [TODO:description]
   *
   * @param value [TODO:parameter]
   */
  virtual void write_int(int value) = 0;
  /**
   * @brief [TODO:description]
   *
   * @param value [TODO:parameter]
   */
  virtual void write_char(char value) = 0;
};

/**
 * @class basic_input
 * @brief [TODO:description]
 *
 */
class basic_input {
public:
  /**
   * @brief [TODO:description]
   */
  virtual std::string read_string() = 0;
  /**
   * @brief [TODO:description]
   *
   * @return [TODO:return]
   */
  virtual int read_int() = 0;
  /**
   * @brief [TODO:description]
   *
   * @return [TODO:return]
   */
  virtual char read_char() = 0;
};
/**
 * @class base_object
 * @brief [TODO:description]
 *
 */
class base_object {
public:
  /**
   * @brief return the string
   */
  [[nodiscard]] virtual std::string to_string() const noexcept;
  /**
   * @brief [TODO:description]
   *
   * @param output
   */
  virtual void to_string(basic_output &output) const noexcept;
  /**
   * @brief [TODO:description]
   */
  [[nodiscard]] virtual const std::type_info &get_type() const noexcept;
};
} // namespace viole
