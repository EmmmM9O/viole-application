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
  virtual auto write_string(const std::string &str) -> void = 0;
  /**
   * @brief [TODO:description]
   *
   * @param value [TODO:parameter]
   */
  virtual auto write_int(int value) -> void = 0;
  /**
   * @brief [TODO:description]
   *
   * @param value [TODO:parameter]
   */
  virtual auto write_char(char value) -> void = 0;
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
  virtual auto read_string() -> std::string = 0;
  /**
   * @brief [TODO:description]
   *
   * @return [TODO:return]
   */
  virtual auto read_int() -> int = 0;
  /**
   * @brief [TODO:description]
   *
   * @return [TODO:return]
   */
  virtual auto read_char() -> char = 0;
};
/**
 * @class basic_object
 * @brief [TODO:description]
 *
 */
class basic_object {
public:
  /**
   * @brief return the string
   */
  [[nodiscard]] virtual auto to_string() const noexcept -> std::string;
  /**
   * @brief [TODO:description]
   *
   * @param output
   */
  virtual auto to_string(basic_output &output) const noexcept -> void;
  /**
   * @brief [TODO:description]
   */
  [[nodiscard]] virtual auto
  get_type() const noexcept -> const std::type_info &;
};
} // namespace viole
