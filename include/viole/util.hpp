#pragma once
#include "viole/base.hpp"
#include "viole/templates.hpp"
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
#include <typeinfo>
namespace viole {
// mate
struct any_type_temp {
  template <typename T> operator T();
};
template <typename T> consteval auto count_member(auto &&...args) -> size_t {
  if constexpr (!requires { T{args...}; }) {
    return sizeof...(args) - 1;
  } else {
    return count_member<T>(args..., any_type_temp{});
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
  explicit enum_define_reflector(string &res) : m_result(res) {
    m_result += '{';
  }
  template <I N> [[nodiscard]] constexpr auto call() const -> bool {
    auto res = c_enum_to_string_v<E, E(N)>();
    if (res == ENUM_OUTOF_RANGE_STRING) {
      m_result[m_result.length() - 1] = '}';
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
  string str = "enum class " + enum_name<E>();
  template_call_loop<std::underlying_type_t<E>, min, max>(
      enum_define_reflector<E>(str));
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
concept is_to_string_std_template = requires(T obj) {
  { std::to_string(obj) } -> std::same_as<std::string>;
};

template <typename T>
concept to_string_template =
    is_base_object_template<T> || is_to_string_parser_template<T> ||
    enum_template<T> || is_to_string_std_template<T>;

template <to_string_template T>
auto to_string(const T &object) -> viole::string {
  if constexpr (is_base_object_template<T>) {
    return object.to_string();
  } else if constexpr (enum_template<T>) {
    return enum_to_string(object);
  } else if constexpr (is_to_string_std_template<T>) {
    return std::to_string(object);
  } else {
    return to_string_parser<T>().to_string(object);
  }
}
auto operator<<(std::ostream &stream,
                const basic_object &object) -> std::ostream &;

//

auto abi_type_info_to_string(const std::type_info &type) -> std::string;
template <typename Type> auto abi_type_to_string() -> std::string {
  return abi::__cxa_demangle(typeid(Type).name(), nullptr, nullptr, nullptr);
}
template <typename T> auto type_to_string() -> std::string {
  std::string str = __PRETTY_FUNCTION__;
  auto first = str.find("T = ");
  first += 4;
  return {str, first, str.length() - first - 1};
}
//
template <typename T>
concept type_pack_template = requires(T) { typename T::current; };
template <typename T>
concept type_pack_empty_template = requires(T) { typename T::empty; };
template <typename T>
concept type_pack_orempty_template =
    type_pack_empty_template<T> || type_pack_template<T>;
template <typename T>
concept type_pack_nextable_template =
    requires(T) { typename T::next; } && type_pack_template<T>;

template <typename T>
concept type_pack_unnextable_template =
    requires(T) { typename T::end; } && type_pack_template<T>;
template <typename Type, typename... Types> struct type_pack {
  using current = Type;
  using next = type_pack<Types...>;
  template <typename Add> using add = type_pack<Type, Types..., Add>;
};

template <typename Type> struct type_pack<Type> {
  using current = Type;
  using end = std::true_type;
  template <typename Add> using add = type_pack<Type, Add>;
};
template <> struct type_pack<void> {
  using empty = std::true_type;
  template <typename Add> using add = type_pack<Add>;
};
using empty_pack = type_pack<void>;
template <typename T, typename... Args>
concept constructable_template = requires(T, Args... args) { T{args...}; };
template <typename S, type_pack_template Pack, typename... T>
struct pack_constructable {};
template <typename S, type_pack_nextable_template Pack, typename... T>
struct pack_constructable<S, Pack, T...> {
  static const constexpr bool value =
      pack_constructable<S, typename Pack::next, T...,
                         typename Pack::current>::value;
};
template <typename S, type_pack_unnextable_template Pack, typename... T>
struct pack_constructable<S, Pack, T...> {
  static const constexpr bool value =
      constructable_template<S, T..., typename Pack::current>;
};
template <type_pack_template A, type_pack_template B> struct combine_pack {};
template <type_pack_template A, type_pack_unnextable_template B>
struct combine_pack<A, B> {
  using type = A::template add<typename B::current>;
};
template <type_pack_template A, type_pack_nextable_template B>
struct combine_pack<A, B> {
  using type = combine_pack<typename A::template add<typename B::current>,
                            typename B::next>::type;
};
template <bool opt, typename True, typename Falee> struct switch_type {};
template <typename Opt1, typename Opt2> struct switch_type<true, Opt1, Opt2> {
  using type = Opt1::type;
};
template <typename Opt1, typename Opt2> struct switch_type<false, Opt1, Opt2> {
  using type = Opt2::type;
};
template <bool opt, typename True, typename Falee> struct switch_type2 {};
template <typename Opt1, typename Opt2> struct switch_type2<true, Opt1, Opt2> {
  using type = Opt1::type;
};
template <typename Opt1, typename Opt2> struct switch_type2<false, Opt1, Opt2> {
  using type = Opt2;
};
} // namespace viole
