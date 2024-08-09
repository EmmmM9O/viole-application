#pragma once
#include "viole/meta.hpp"
#include "viole/util.hpp"
#include <array>
#include <iostream>
#include <map>
#include <sstream>
#include <type_traits>
namespace viole {
template <typename T> struct json_writer {
  // auto write_json(const T &&) -> std::string = delete;
};

template <typename T>
concept json_write_singal_able_template = requires(T &&obj) {
  { json_writer<T>().write_json(obj) } -> std::same_as<std::string>;
};
template <typename T>
concept json_write_able_template =
    json_write_singal_able_template<T> || meta_object_template<T>;
template <> struct json_writer<int> {
  auto write_json(int &obj) -> std::string { return to_string(obj); }
};
template <> struct json_writer<float> {
  auto write_json(float &obj) -> std::string { return to_string(obj); }
};
template <> struct json_writer<double> {
  auto write_json(double &obj) -> std::string { return to_string(obj); }
};
template <> struct json_writer<std::string> {
  auto write_json(std::string &obj) -> std::string { return "\"" + obj + "\""; }
};
template <json_write_able_template T, size_t size>
struct json_writer<std::array<T, size>> {
  auto write_json(std::array<T, size> &arr) -> std::string {
    if (size == 0) {
      return "[]";
    }
    std::stringstream stream{""};
    stream << "[" << json_writer<T>().write_json(arr[0]);
    for (int i = 1; i < size; i++) {
      stream << "," << json_writer<T>().write_json(arr[i]);
    }
    stream << "]";
    return stream.str();
  }
};
template <json_write_able_template T> struct json_writer<std::vector<T>> {
  auto write_json(std::vector<T> &arr) -> std::string {
    if (arr.size() == 0) {
      return "[]";
    }
    std::stringstream stream{""};
    stream << "[" << json_writer<T>().write_json(arr[0]);
    for (int i = 1; i < arr.size(); i++) {
      stream << "," << json_writer<T>().write_json(arr[i]);
    }
    stream << "]";
    return stream.str();
  }
};
template <json_write_able_template T>
struct json_writer<std::map<std::string, T>> {
  auto write_json(std::map<std::string, T> map) -> std::string {
    if (map.empty()) {
      return "{}";
    }
    std::stringstream stream{""};
    for (auto [key, value] : map) {
      stream << "\"" << key << "\":" << json_writer<T>().write_json(value);
    }
  }
};
template <typename T>
auto obj_to_string_json(T &&obj, bool pretty, const char *field_name = "",
                        int depth = 0) {
  // static_assert(!json_write_able_template<std::decay_t<T>>,
  //              "un json write able");
  std::stringstream str;
  auto indent = [depth, &str, pretty] {
    if (pretty) {
      for (int i = 0; i < depth; ++i) {
        str << "    ";
      }
    }
  };

  if constexpr (meta_object_template<T>) {
    indent();
    str << (*field_name ? "\"" : "") << field_name << (*field_name ? "\"" : "")
        << (*field_name ? ":{" : "{");
    if (pretty) {
      str << std::endl;
    }
    meta_obj_for_each(
        obj, [depth, pretty, &str](auto &&field_name, auto &&value) {
          str << obj_to_string_json(value, pretty, field_name, depth + 1);
        });

    auto tmp_str = str.str();
    tmp_str.pop_back();
    if (pretty) {
      tmp_str.pop_back();
      tmp_str += "\n";
    }
    str.str(tmp_str);
    str.seekp(0, std::ios_base::end);
    indent();
    str << "}" << (depth == 0 ? "" : ",");
    if (pretty) {
      str << std::endl;
    }
  } else {
    indent();
    str << "\"" << field_name << "\"" << ":"
        << json_writer<std::decay_t<T>>().write_json(obj) << ",";
    if (pretty) {
      str << std::endl;
    }
  }
  return str.str();
}
template <typename T> auto obj_to_json(T &&obj, bool pretty) {
  return obj_to_string_json(obj, pretty);
}
template <meta_object_template T> struct json_writer<T> {
  static auto write_json(const T &&obj) -> std::string {
    return obj_to_json(obj, false);
  }
};
} // namespace viole
