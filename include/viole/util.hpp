#pragma once
#include "viole/base.hpp"
#include "viole/stl.hpp"
#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>
namespace viole {

template <typename Type>
concept enum_template = std::is_enum<Type>::value;

template <typename T>
concept is_base_object_template =
    std::is_base_of<viole::basic_object, T>::value;
template <typename T> struct to_string_parser {
  to_string_parser();
  [[nodiscard]] auto
  to_string(const T &) const noexcept -> viole::string = delete;
};
template <> struct to_string_parser<int> {
  [[nodiscard]] auto to_string(int value) const noexcept -> viole::string {
    return std::to_string(value);
  }
};
template <> struct to_string_parser<float> {
  [[nodiscard]] auto to_string(float value) const noexcept -> viole::string {
    return std::to_string(value);
  }
};
template <> struct to_string_parser<double> {
  [[nodiscard]] auto to_string(double value) const noexcept -> viole::string {
    return std::to_string(value);
  }
};
template <typename T>
concept is_to_string_parser_template = requires(T obj) {
  { to_string_parser<T>().to_string(obj) } -> std::same_as<viole::string>;
};

template <typename T>
concept to_string_template =
    is_base_object_template<T> || is_to_string_parser_template<T> ||
    enum_template<T>;

template <to_string_template T>
auto to_string(const T &object) -> viole::string {
  if constexpr (is_base_object_template<T>) {
    return object.to_string();
  } else {
    return to_string_parser<T>().to_string(object);
  }
}

auto operator<<(std::ostream &stream,
                const basic_object &object) -> std::ostream &;
} // namespace viole
