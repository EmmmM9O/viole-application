#pragma once
#include "viole/base.hpp"
#include "viole/stl.hpp"
#include <concepts>
#include <type_traits>
namespace viole {
template <typename T>
concept is_base_object = std::is_base_of<viole::basic_object, T>::value;
template <typename T> struct to_string_parser {
  [[nodiscard]] auto
  to_string(const T &) const noexcept -> viole::string = delete;
};
template <typename T>
concept is_to_string_parser_able = requires(T obj) {
  { to_string_parser<T>().to_string(obj) } -> std::same_as<viole::string>;
};
template <typename T>
concept to_string_able = is_base_object<T> || is_to_string_parser_able<T>;

template <to_string_able T> auto to_string(const T &object) -> viole::string {
  if constexpr (is_base_object<T>) {
    return object.to_string();
  } else {
    return to_string_parser<T>().to_string(object);
  }
}
} // namespace viole
