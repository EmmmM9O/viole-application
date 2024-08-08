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

template <type_pack_orempty_template Tmp, type_pack_template Types,
          type_pack_template Now, typename S>
struct get_constructor {};
template <type_pack_orempty_template Tmp, type_pack_template Types,
          type_pack_unnextable_template Now, typename S>
struct get_constructor<Tmp, Types, Now, S> {
  static const constexpr bool constructable = pack_constructable<
      S, typename Tmp::template add<typename Now::current>>::value;
  using type = switch_type2<
      constructable,
      get_constructor<typename Tmp::template add<typename Now::current>, Types,
                      Types, S>,
      Tmp>::type;
};

template <type_pack_orempty_template Tmp, type_pack_template Types,

          type_pack_nextable_template Now, typename S>
struct get_constructor<Tmp, Types, Now, S> {
  static const constexpr bool constructable = pack_constructable<
      S, typename Tmp::template add<typename Now::current>>::value;
  using type = switch_type<
      constructable,
      get_constructor<typename Tmp::template add<typename Now::current>, Types,
                      Types, S>,
      get_constructor<Tmp, Types, typename Now::next, S>>::type;
};
template <type_pack_template Types, typename S>
using get_constructor_t = get_constructor<empty_pack, Types, Types, S>::type;
template <type_pack_template Types, typename S>
using get_tuple_struct_t = pack_tuple<get_constructor_t<Types,S>>::type;
} // namespace viole
