module;
#include <any>
#include <string>
#include <typeinfo>
export module viole_argparser;
import viole_util;

export namespace viole {
// option
template <template_enum Type, typename Context, typename Extra>
struct option_parser {
  option_parser(Type type, Extra extra) = delete;
  std::any parse(Context context) = delete;
  void point(any_point &point, std::any result) { point.set_any(result); }
};
template <typename T, typename Context, typename Extra>
concept template_option_type_enum =
    requires(T t, Context context, Extra extra, any_point &point,
             std::any result, const std::type_info &type) {
      template_enum<T>;
      {
        option_parser<T, Context, Extra>(t, extra)
      } -> std::same_as<option_parser<T, Context, Extra>>;
      {
        option_parser<T, Context, Extra>(t, extra).parse(context)
      } -> std::same_as<std::any>;
      {
        option_parser<T, Context, Extra>(t, extra).point(point, result)
      } -> std::same_as<void>;
      {
        option_parser<T, Context, Extra>(t, extra).check_type(type)
      } -> std::convertible_to<bool>;
    };
enum class standard_option_type_enum {
  Int,
  String,
};
enum class standard_option_type_extra {
  Array,
  Default,
};
template <>
struct option_parser<standard_option_type_enum, std::string &,
                     standard_option_type_extra> {
  standard_option_type_enum type;
  standard_option_type_extra extra;
  option_parser(standard_option_type_enum type,
                standard_option_type_extra extra)
      : type(type), extra(extra) {}

  std::any parse(std::string &context) {

  };
  void point(any_point point, std::any result) { point.set_any(result); }
  constexpr bool check_type(const std::type_info &type_info) {
    switch (extra) {
    case standard_option_type_extra::Default:
      switch (type) {
      case standard_option_type_enum::Int:
        if(type_info==typeid(int)) return true;
        break;
      case standard_option_type_enum::String:
        if(type_info==typeid(std::string)) return true;
        break;
      }
      break;
    case standard_option_type_extra::Array:
      switch (type) {
      case standard_option_type_enum::Int:
        if(type_info==typeid(std::vector<int>)) return true;
        break;
      case standard_option_type_enum::String:
        if(type_info==typeid(std::vector<std::string>)) return true;
        break;
      }
      break;
    }
    return false;
  }
};
static_assert(
    template_option_type_enum<standard_option_type_enum, std::string &,
                              standard_option_type_extra>,
    "error standard_option class dose not fit the template");
template <typename Context, typename String, typename Extra,
          template_option_type_enum<Context, Extra> Type>
class option {
public:
  String name;
  Type type;
  Extra extra;

private:
  std::any data;
  any_point point;
  option_parser<Type, String, Extra> parser;

public:
  [[nodiscard]] std::any get_data_any() const { return data; }
  template <typename T> [[nodiscard]] T get_data() {
    return std::any_cast<T>(data);
  }
  void parse(const Context &context) {
    data = parser.parse(context, type);
    if (point.has_value())
      parser.point(point, data);
  };
  option(String name, Type type, Extra extra)
      : name(name), type(type), extra(extra), parser(type, extra) {}
  template <typename T>
  option(String name, Type type, Extra extra, T *ref)
      : name(name), type(type), extra(extra), point(ref), parser(type, extra) {
    static_assert(parser.check_type(typeid(T)), "error any point type");
  }
};
using standard_option =
    option<std::string &, const char *, standard_option_type_extra,
           standard_option_type_enum>;

} // namespace viole
