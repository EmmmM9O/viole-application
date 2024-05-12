module;
#include <any>
#include <concepts>
#include <string>
export module viole_argparser;
import viole_util;

export namespace viole {
// option
template <template_enum Type, typename Context, typename Extra>
struct option_parser {
  option_parser(Extra extra) = delete;
  std::any parse(Context context, Type t) = delete;
};
template <typename T, typename Context, typename Extra>
concept template_option_type_enum =
    requires(T t, Context context, Extra extra) {
      template_enum<T>;
      option_parser<T, Context, Extra>(extra);
      option_parser<T, Context, Extra>(extra).parse(context, t);
    };
enum class standard_option_type_enum {
  Int,
  String,
};
enum class standard_option_type_extra {
  Array,
  Path,
  Default,
};
template <>
struct option_parser<standard_option_type_enum, const std::string &,
                     standard_option_type_extra> {
  option_parser(standard_option_type_extra) {}

  std::any parse(const std::string &context, standard_option_type_enum t) {

  };
};
static_assert(
    template_option_type_enum<standard_option_type_enum, const std::string &,
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

public:
  [[nodiscard]] std::any get_data_any() const { return data; }
  template <typename T> [[nodiscard]] T get_data() {
    return std::any_cast<T>(data);
  }
  void parse(const Context &context) {
    data = option_parser<Type, String, Extra>().parse(context, type);
  }
  option(String name, Type type, Extra extra)
      : name(name), type(type), extra(extra) {}
  template <typename T>
  option(String name, Type type, Extra extra, T *ref)
      : name(name), type(type), extra(extra) {}
};
} // namespace viole
