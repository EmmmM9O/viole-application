/**
 * @file
 * @brief basic types
 * @author novarc
 * @date 2024-5-19
 */

#pragma once
#include <typeinfo>
#include <vector>

namespace viole {
using string = std::string;
template <typename T> using vector = std::vector<T>;
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
  virtual auto write_string(const viole::string &str) -> void = 0;
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
  virtual auto read_string() -> viole::string = 0;
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
  basic_object() = default;
  basic_object(const basic_object &) = default;
  basic_object(basic_object &&) = delete;
  auto operator=(const basic_object &) -> basic_object & = default;
  auto operator=(basic_object &&) -> basic_object & = delete;
  virtual ~basic_object() = default;
  /**
   * @brief return the string
   */
   [[viole::must_override]] [[nodiscard]] virtual auto to_string() const noexcept -> viole::string;
  [[nodiscard]] virtual auto to_string_full() const noexcept -> viole::string;

  /**
   * @brief [TODO:description]
   *
   * @param output
   */
  virtual auto to_string(basic_output &output) const noexcept -> void;
  /**
   * @brief
   */
  [[viole::must_override]] [[nodiscard]] virtual auto
  get_type() const noexcept -> const std::type_info &;
 [[nodiscard]] virtual auto
  get_type_name() const noexcept -> const char *;
};
} // namespace viole
