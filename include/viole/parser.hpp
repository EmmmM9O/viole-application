/**
 * @file format
 * @brief format
 * @author novarc
 * @date 2024-6-25
 */
#pragma once
#include "viole/templates.hpp"
#include <concepts>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
namespace viole {
template <typename T, typename Result, typename Input>
concept parser_template = requires(T obj, Input input) {
  { obj(input) } -> std::same_as<std::optional<std::pair<Result, Input>>>;
};
template <typename T, typename Input>
concept parser_t_template = requires(T obj, Input input) {
  {
    std::invoke_result_t<T, Input>::value_type::second_type
  } -> std::same_as<Input>;
};
template <typename Input> struct parser {
  using input_type = Input;
  template <typename Result>
  using result_type = std::optional<std::pair<Result, input_type>>;
  template <typename Result>
  using function_type = auto (*)(input_type) -> result_type<Result>;

  constexpr auto make_char_parser(char cha) {
    return [=](input_type input) -> result_type<char> {
      if (input.empty() || cha != input[0]) {
        return std::nullopt;
      }
      return std::make_pair(input[0],
                            input_type(input.begin() + 1, input.size() - 1));
    };
  }
  constexpr auto one_of(input_type chars) {
    return [=](input_type input) -> result_type<char> {
      if (input.empty() || chars.find(input) == std::string::npos) {
        return std::nullopt;
      }
      return std::make_pair(input[0],
                            input_type(input.begin() + 1, input.size() - 1));
    };
  }
  template <typename Parser> struct parser_type {
    using result = std::invoke_result_t<Parser, input_type>;
    using type = result::value_type::first_type;
  };
  template <typename Parser> using parser_t = parser_type<Parser>::type;
  template <typename P1, typename P2, typename F,
            typename R = std::invoke_result_t<F, parser_t<P1>, parser_t<P2>>>
  constexpr auto combine(P1 &&parser1, P2 &&parser2, F &&func) {
    return [=](input_type input) -> result_type<R> {
      auto result1 = parser1(input);
      if (!result1) {
        return std::nullopt;
      }
      auto result2 = parser2(result1->second);
      if (!result2) {
        return std::nullopt;
      }
      return std::make_pair(func(result1->first, result2->first),
                            result2->second);
    };
  };

  template <typename R, typename P, func2_template<R, R, parser_t<P>> F>
  constexpr auto fold(P &&parser, R acc, F &&func,
                      input_type input) -> result_type<R> {
    while (true) {
      auto res = parser(input);
      if (!res) {
        return std::make_pair(acc, input);
      }
      acc = func(acc, res->first);
      input = res->second;
    }
  }
  template <typename P> constexpr auto many(P &&parser) {
    return [parser = std::forward<P>(parser)](
               input_type input) -> result_type<std::monostate> {
      return fold(
          parser, std::monostate{}, [](auto acc, auto) { return acc; }, input);
    };
  }
  template <typename P, typename R, func2_template<R, R, parser_t<P>> F>
  constexpr auto at_least(P &&parser, R &&init, F &&func) {
    static_assert(std::is_same_v<std::invoke_result_t<F, R, parser_t<P>>, R>,
                  "type mismatch");
    return [parser = std::forward<P>(parser), func = std::forward<F>(func),
            init = std::forward<R>(init)](input_type input) -> result_type<R> {
      auto res = parser(input);
      if (!res) {
        return std::nullopt;
      }
      return viole::parser<R>::fold(parser, func(init, res->first), func,
                                    res->second);
    };
  }
  template <typename P, typename R = parser_t<P>>
  constexpr auto option(P &&parser, R &&default_v) {
    return [=](input_type input) -> result_type<R> {
      auto res = parser(input);
      if (!res) {
        return std::make_pair(default_v, input);
      }
      return res;
    };
  }
};
template <typename P1, typename P2>
constexpr auto operator>(P1 &&parser1, P2 &&parser2) {
  return combine(std::forward<P1>(parser1), std::forward<P2>(parser2),
                 [](auto &&left, auto) { return left; });
}

template <typename P1, typename P2>
constexpr auto operator<(P1 &&parser1, P2 &&parser2) {
  return combine(std::forward<P1>(parser1), std::forward<P2>(parser2),
                 [](auto, auto &&right) { return right; });
}

//};
} // namespace viole
