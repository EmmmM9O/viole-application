
#pragma once
#include <algorithm>
#include <concepts>
#include <cstddef>
namespace viole {

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
constexpr auto template_call_loop(const F &func) -> void {
  if (!func.template call<begin>()) {
    if constexpr (begin != end) {
      template_call_loop<I, begin + 1, end>(func);
    }
  }
}
} // namespace viole

// NOLINTBEGIN
#define TEMPLATE_FIT(__TYPE, __TEMPLATE)                                       \
  static_assert(__TEMPLATE<__TYPE>, #__TYPE " not fit the " #__TEMPLATE)
// NOLINTEND
