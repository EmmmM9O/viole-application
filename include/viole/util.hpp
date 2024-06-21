#pragma once
#include "viole/base.hpp"
#include "viole/stl.hpp"
#include <algorithm>
#include <concepts>
#include <cstring>
#include <cxxabi.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace viole {
// NOLINTBEGIN
#define TEMPLATE_FIT(__TYPE, __TEMPLATE)                                       \
  static_assert(__TEMPLATE<__TYPE>, #__TYPE " not fit the " #__TEMPLATE)
// NOLINTEND
template <std::size_t N> struct template_string {
  constexpr template_string(const char (&arr)[N]) {
    std::copy(arr, arr + N, m_string);
  }
  char m_string[N]{};
};

template <typename T>
concept runnable_template = requires(T func) {
  { func() } -> std::same_as<void>;
};
template <typename T>
concept callable_template = requires(const T &func) {
  { func.call() } -> std::same_as<void>;
};
// loop
template <typename T>
concept loop_index_template = requires(T index) {
  { index + 1 } -> std::same_as<T>;
};
template <typename T>
concept loop_callable_template = requires(const T &func) {
  { func.template call<0>() } -> std::same_as<bool>;
};
template <loop_index_template I, I begin, I end, loop_callable_template F>
constexpr typename std::enable_if<begin == end>::type
template_call_loop(const F &func) {
  func.template call<begin>();
}
template <loop_index_template I, I begin, I end, loop_callable_template F>
constexpr typename std::enable_if<begin != end>::type
template_call_loop(const F &func) {
  if (!func.template call<begin>()) {
    template_call_loop<I, begin + 1, end>(func);
  }
}
// Enum
template <typename Type>
concept enum_template = std::is_enum<Type>::value;
template <enum_template E, E V>
constexpr auto c_enum_to_string() -> viole::string {
  viole::string tmp = __PRETTY_FUNCTION__;
  const char *pre = "V = ";
  auto first = tmp.find(pre);
  first += strlen(pre);
  return {tmp, first, tmp.length() - first - 1};
}
#define ENUM_OUTOF_RANGE_STRING "@out@"
template <enum_template E, E V>
constexpr auto c_enum_to_string_v() -> viole::string {
  viole::string tmp = __PRETTY_FUNCTION__;
  auto first = tmp.find("V = ");
  auto second = tmp.find("::", first);
  if (second == string::npos) {
    return ENUM_OUTOF_RANGE_STRING;
  }
  second += 2;
  return {tmp, second, tmp.length() - second - 1};
}
#ifndef ENUM_SERIALIZER_ALIGNED
#define ENUM_SERIALIZER_ALIGNED 16
#endif
template <enum_template E> struct enum_serializer {
private:
  string &m_str;
  std::underlying_type_t<E> m_n;

public:
  enum_serializer(std::underlying_type_t<E> n, string &str)
      : m_n(n), m_str(str) {}
  template <std::underlying_type_t<E> N>
  [[nodiscard]] constexpr auto call() const -> bool {
    if (m_n == N) {
      m_str = c_enum_to_string<E, E(N)>();
      return true;
    }
    return false;
  }
} __attribute__((aligned(ENUM_SERIALIZER_ALIGNED)));
#ifndef ENUM_FINDER_ALIGNED
#define ENUM_FINDER_ALIGNED 32
#endif
template <enum_template E> struct enum_finder {
private:
  std::string_view m_target;
  std::underlying_type_t<E> &m_n;

public:
  enum_finder(std::underlying_type_t<E> &n, std::string_view target)
      : m_target(target), m_n(n) {}
  template <std::underlying_type_t<E> N>
  [[nodiscard]] constexpr auto call() const -> bool {
    auto res = c_enum_to_string_v<E, E(N)>();
    if (res == ENUM_OUTOF_RANGE_STRING) {
      return true;
    }
    if (res == m_target) {
      m_n = N;
      return true;
    }
    return false;
  }
} __attribute__((aligned(ENUM_FINDER_ALIGNED)));
template <enum_template E> struct enum_key_reflector {
private:
  using I = std::underlying_type_t<E>;
  std::vector<I> &m_result;

public:
  explicit enum_key_reflector(std::vector<I> &res) : m_result(res) {}
  template <I N> [[nodiscard]] constexpr auto call() const -> bool {
    if (c_enum_to_string_v<E, E(N)>() == ENUM_OUTOF_RANGE_STRING) {
      return true;
    }
    m_result.push_back(N);
    return false;
  }
};
template <enum_template E> struct enum_value_reflector {
private:
  using I = std::underlying_type_t<E>;
  std::vector<string> &m_result;

public:
  explicit enum_value_reflector(std::vector<string> &res) : m_result(res) {}
  template <I N> [[nodiscard]] constexpr auto call() const -> bool {
    auto res = c_enum_to_string_v<E, E(N)>();
    if (res == ENUM_OUTOF_RANGE_STRING) {
      return true;
    }
    m_result.push_back(res);
    return false;
  }
};
template <enum_template E> struct enum_reflector {
private:
  using I = std::underlying_type_t<E>;
  std::map<I, string> &m_result;

public:
  explicit enum_reflector(std::map<I, string> &res) : m_result(res) {}
  template <I N> [[nodiscard]] constexpr auto call() const -> bool {
    auto res = c_enum_to_string_v<E, E(N)>();
    if (res == ENUM_OUTOF_RANGE_STRING) {
      return true;
    }
    m_result[N] = res;
    return false;
  }
};
template <enum_template E> struct enum_sizer {
private:
  using I = std::underlying_type_t<E>;
  I &m_result;

public:
  explicit enum_sizer(I &res) : m_result(res) {}
  template <I N> [[nodiscard]] constexpr auto call() const -> bool {
    auto res = c_enum_to_string_v<E, E(N)>();
    if (res == ENUM_OUTOF_RANGE_STRING) {
      return true;
    }
    m_result++;
    return false;
  }
};
template <enum_template E> struct enum_define_reflector {
private:
  using I = std::underlying_type_t<E>;
  string &m_result;

public:
  explicit enum_define_reflector(string &res) : m_result(res) {}
  template <I N> [[nodiscard]] constexpr auto call() const -> bool {
    auto res = c_enum_to_string_v<E, E(N)>();
    if (res == ENUM_OUTOF_RANGE_STRING) {
      return true;
    }
    m_result += std::to_string(N) + ":" + res + ",";
    return false;
  }
};
enum util_static_assert_enum {

};
TEMPLATE_FIT(enum_serializer<util_static_assert_enum>, loop_callable_template);
TEMPLATE_FIT(enum_finder<util_static_assert_enum>, loop_callable_template);
TEMPLATE_FIT(enum_reflector<util_static_assert_enum>, loop_callable_template);
TEMPLATE_FIT(enum_key_reflector<util_static_assert_enum>,
             loop_callable_template);
TEMPLATE_FIT(enum_value_reflector<util_static_assert_enum>,
             loop_callable_template);
TEMPLATE_FIT(enum_define_reflector<util_static_assert_enum>,
             loop_callable_template);
TEMPLATE_FIT(enum_sizer<util_static_assert_enum>, loop_callable_template);
#ifndef ENUM_RANGE_MIN
#define ENUM_RANGE_MIN (0)
#endif
#ifndef ENUM_RANGE_MAX
#define ENUM_RANGE_MAX (64)
#endif
template <enum_template E> auto enum_name() -> string {
  return abi::__cxa_demangle(typeid(E).name(), nullptr, nullptr, nullptr);
}
template <enum_template E, std::underlying_type_t<E> min = ENUM_RANGE_MIN,
          std::underlying_type_t<E> max = ENUM_RANGE_MAX>
auto enum_class_to_string() -> viole::string {
  string str = "enum class " + enum_name<E>() + "{";
  template_call_loop<std::underlying_type_t<E>, min, max>(
      enum_define_reflector<E>(str));
  str += "}";
  return str;
}
template <enum_template E, std::underlying_type_t<E> min = ENUM_RANGE_MIN,
          std::underlying_type_t<E> max = ENUM_RANGE_MAX>
auto enum_to_string(E enum_obj) -> viole::string {
  string str;
  template_call_loop<std::underlying_type_t<E>, min, max>(enum_serializer<E>(
      static_cast<std::underlying_type_t<E>>(enum_obj), str));
  if (str.empty()) {
    throw std::runtime_error("enum out of range");
  }

  return str;
}
template <enum_template E, std::underlying_type_t<E> min = ENUM_RANGE_MIN,
          std::underlying_type_t<E> max = ENUM_RANGE_MAX>
auto enum_to_index(std::string_view value) -> std::underlying_type_t<E> {
  std::underlying_type_t<E> index = ENUM_RANGE_MIN - 1;
  template_call_loop<std::underlying_type_t<E>, min, max>(
      enum_finder<E>(index, value));
  if (index == ENUM_RANGE_MIN - 1) {
    throw std::runtime_error("enum not find");
  }
  return index;
}
template <enum_template E, std::underlying_type_t<E> min = ENUM_RANGE_MIN,
          std::underlying_type_t<E> max = ENUM_RANGE_MAX>
auto enum_keys(std::vector<std::underlying_type_t<E>> &res) -> void {
  template_call_loop<std::underlying_type_t<E>, min, max>(
      enum_key_reflector<E>(res));
}
template <enum_template E, std::underlying_type_t<E> min = ENUM_RANGE_MIN,
          std::underlying_type_t<E> max = ENUM_RANGE_MAX>
auto enum_values(std::vector<string> &res) -> void {
  template_call_loop<std::underlying_type_t<E>, min, max>(
      enum_value_reflector<E>(res));
}
template <enum_template E, std::underlying_type_t<E> min = ENUM_RANGE_MIN,
          std::underlying_type_t<E> max = ENUM_RANGE_MAX>
auto enum_map(std::map<std::underlying_type_t<E>, string> &res) -> void {
  template_call_loop<std::underlying_type_t<E>, min, max>(
      enum_reflector<E>(res));
}
template <enum_template E, std::underlying_type_t<E> min = ENUM_RANGE_MIN,
          std::underlying_type_t<E> max = ENUM_RANGE_MAX>
auto enum_max_size() -> std::underlying_type_t<E> {
  std::underlying_type_t<E> size = 0;
  template_call_loop<std::underlying_type_t<E>, min, max>(enum_sizer<E>(size));
  return size;
}
//

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
  } else if constexpr (enum_template<T>) {
    return enum_to_string(object);
  } else {
    return to_string_parser<T>().to_string(object);
  }
}

auto operator<<(std::ostream &stream,
                const basic_object &object) -> std::ostream &;
} // namespace viole
