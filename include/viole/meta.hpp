#pragma once
#include "viole/templates.hpp"
#include "viole/util.hpp"
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>
namespace viole {
struct type_meta {
  std::vector<type_meta> contexts;
  const std::type_info &type;
};
template <typename Type> struct basic_type_builder {
  template <typename T>
  static auto build(type_meta &meta, auto &&...args) -> std::optional<Type> {
    if constexpr ((requires { T{args..., Type()}; })) {
      meta.contexts.push_back({{}, typeid(Type)});
      return Type();
    }
    return std::nullopt;
  }
};

template <typename Type> struct detail_type_builder {
  static type_meta detail_type;
  template <typename T>
  static auto build(type_meta &meta, auto &&...args) -> std::optional<Type> {
    if constexpr ((requires { T{args..., Type()}; })) {
      meta.contexts.push_back(detail_type);
      return Type();
    }
    return std::nullopt;
  }
};

template <typename T, typename... TypeBuilders> struct type_meta_builder {
  static auto get() -> type_meta {
    type_meta_builder<T, TypeBuilders...> builder{};
    return builder.get_obj();
  }
  type_meta meta = {{}, typeid(T)};
  auto get_obj(auto &&...args) -> type_meta {
    loop(args...);
    return meta;
  }
  auto loop(auto &&...args) -> void {
    (
        [&]() {
          auto opt = TypeBuilders::template build<T>(meta, args...);
          if (opt.has_value()) {
            if constexpr ((requires { T{args..., *opt}; })) {
              loop(args..., *opt);
              return;
            }
          }
        }(),
        ...);
  }
};
using default_type_pack =
    type_pack<basic_type_builder<int>, basic_type_builder<double>,
              basic_type_builder<std::string>, basic_type_builder<char>>;
TEMPLATE_FIT(default_type_pack, type_pack_template);
template <typename T, type_pack_template Pack> struct fold_meta_type_builder {
  template <typename Current = Pack, typename... Args>
  static auto get() -> type_meta {
    if constexpr (requires { typename Current::next; }) {
      return get<typename Current::next, Args..., typename Current::current>();
    } else {
      return type_meta_builder<T, Args...>::get();
    }
  }
};
template <typename T, type_pack_template Pack>
auto type_meta_to_string(
    int depth = 0,
    type_meta meta = fold_meta_type_builder<T, Pack>::get()) -> std::string {
  std::stringstream stream;

  auto indent = [depth, &stream] {
    for (int i = 0; i < depth; ++i) {
      stream << "    ";
    }
  };
  indent();
  stream << abi_type_info_to_string(meta.type);
  if (!meta.contexts.empty()) {
    stream << ":{" << std::endl;
    for (auto child : meta.contexts) {
      stream << type_meta_to_string<T, Pack>(depth + 1, child);
    }
    stream << "}" << (depth == 0 ? "" : ",") << std::endl;
  } else {
    stream << "," << std::endl;
  }
  return stream.str();
}

template <typename T>
concept meta_object_template =
    requires(T) { typename std::decay_t<T>::meta_mark; };
#define GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13,    \
                    _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24,     \
                    _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35,     \
                    _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46,     \
                    _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57,     \
                    _58, _59, _60, _61, _62, _63, _64, n, ...)                 \
  n

#define GET_ARG_COUNT(...)                                                     \
  GET_NTH_ARG(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, \
              51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36,  \
              35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,  \
              19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2,  \
              1)
#define DEFINE_STRUCT(st, ...)                                                 \
  struct st {                                                                  \
    using meta_mark = std::true_type;                                          \
    template <typename, size_t> struct FIELD;                                  \
    static constexpr size_t _field_count_ = GET_ARG_COUNT(__VA_ARGS__);        \
    static constexpr const char *_struct_name = #st;                           \
    PASTE(FOR_EACH_, GET_ARG_COUNT(__VA_ARGS__))(FIELD_EACH, 0, __VA_ARGS__)   \
  };                                                                           \
  TEMPLATE_FIT(st, ::viole::meta_object_template);

#define FIELD_EACH(i, arg)                                                     \
  PAIR(arg);                                                                   \
  template <typename T> struct FIELD<T, i> {                                   \
    T &obj;                                                                    \
    FIELD(T &obj) : obj(obj) {}                                                \
    auto value() -> decltype(auto) { return (obj.STRIP(arg)); }                \
    static constexpr auto name() -> const char * {                             \
      return STRING(STRIP(arg));                                               \
    }                                                                          \
  }
// PAIR((double) x NECESSARY)
#define PAIR(x) PARE x // PAIR((double) x) => PARE(double) x => double xx
#define PARE(...) __VA_ARGS__
#define STRIP(x) EAT x // STRIP((double) x) => EAT(double) x =>  x
#define EAT(...)
#define STRING(x) STR(x)
#define STR(x) #x
#define PASTE(x, y) CONCATE(x, y)
#define CONCATE(x, y) x##y
#define FOR_EACH_1(func, i, arg) func(i, arg);
#define FOR_EACH_2(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_1(func, i + 1, __VA_ARGS__)
#define FOR_EACH_3(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_2(func, i + 1, __VA_ARGS__)
#define FOR_EACH_4(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_3(func, i + 1, __VA_ARGS__)
#define FOR_EACH_5(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_4(func, i + 1, __VA_ARGS__)
#define FOR_EACH_6(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_5(func, i + 1, __VA_ARGS__)
#define FOR_EACH_7(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_6(func, i + 1, __VA_ARGS__)
#define FOR_EACH_8(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_7(func, i + 1, __VA_ARGS__)
#define FOR_EACH_9(func, i, arg, ...)                                          \
  func(i, arg);                                                                \
  FOR_EACH_8(func, i + 1, __VA_ARGS__)
#define FOR_EACH_10(func, i, arg, ...)                                         \
  func(i, arg);                                                                \
  FOR_EACH_9(func, i + 1, __VA_ARGS__)
#define FOR_EACH_11(func, i, arg, ...)                                         \
  func(i, arg);                                                                \
  FOR_EACH_10(func, i + 1, __VA_ARGS__)
#define FOR_EACH_12(func, i, arg, ...)                                         \
  func(i, arg);                                                                \
  FOR_EACH_11(func, i + 1, __VA_ARGS__)
#define FOR_EACH_13(func, i, arg, ...)                                         \
  func(i, arg);                                                                \
  FOR_EACH_12(func, i + 1, __VA_ARGS__)
#define FOR_EACH_13(func, i, arg, ...)                                         \
  func(i, arg);                                                                \
  FOR_EACH_12(func, i + 1, __VA_ARGS__)

//...

template <meta_object_template T, typename F, size_t... Is>
inline constexpr void
meta_obj_for_each(T &&obj, F &&func,
                  std::index_sequence<Is...> index_sequence) {
  using TDECAY = std::decay_t<T>;
  (void(func(typename TDECAY::template FIELD<TDECAY, Is>(obj).name(),
             typename TDECAY::template FIELD<TDECAY, Is>(obj).value())),
   ...);
}

template <meta_object_template T, typename F>
inline constexpr void meta_obj_for_each(T &&obj, F &&func) {
  meta_obj_for_each(std::forward<T>(obj), std::forward<F>(func),
                    std::make_index_sequence<std::decay_t<T>::_field_count_>{});
}
template <typename T>
auto meta_obj_to_string(T &&obj, const char *field_name = "",
                        int depth = 0) -> std::string {
  std::stringstream str;
  auto indent = [depth, &str] {
    for (int i = 0; i < depth; ++i) {
      str << "    ";
    }
  };

  if constexpr (meta_object_template<T>) {
    indent();
    str << field_name << (*field_name ? ": {" : "{") << std::endl;
    meta_obj_for_each(obj, [depth, &str](auto &&field_name, auto &&value) {
      str << meta_obj_to_string(value, field_name, depth + 1);
    });
    indent();
    str << "}" << (depth == 0 ? "" : ",") << std::endl;
  } else {
    indent();
    str << field_name << ": " << obj << "," << std::endl;
  }
  return str.str();
}
} // namespace viole
