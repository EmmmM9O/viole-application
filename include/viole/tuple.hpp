#pragma once
#include "viole/util.hpp"
#include <tuple>
#include <type_traits>
namespace viole {
template <type_pack_template Pack, typename... T> struct pack_tuple {};
template <type_pack_nextable_template Pack, typename... T>
struct pack_tuple<Pack, T...> {
  using type =
      pack_tuple<typename Pack::next, T..., typename Pack::current>::type;
};
template <type_pack_unnextable_template Pack, typename... T>
struct pack_tuple<Pack, T...> {
  using type = std::tuple<T..., typename Pack::current>;
};

template <type_pack_template Tmp, type_pack_template Types,
          type_pack_template Now, typename S>
struct get_constructor {};
template <type_pack_template Tmp, type_pack_template Types,
          type_pack_unnextable_template Now, typename S>
struct get_constructor<Tmp, Types, Now, S> {
  static const constexpr bool constructable =
      pack_constructable<S, Tmp, typename Now::current>::value;
};

} // namespace viole
