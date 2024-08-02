#include "viole/base.hpp"
#include <cxxabi.h>
#include <string>
namespace viole {
[[nodiscard]] auto
basic_object::get_type() const noexcept -> const std::type_info & {
  return typeid(basic_object);
}

auto basic_object::to_string(basic_output &output) const noexcept -> void {
  output.write_string(this->to_string());
}
[[nodiscard]] auto basic_object::to_string() const noexcept -> viole::string {
  return "{}";
}
[[nodiscard]] auto
basic_object::to_string_full() const noexcept -> viole::string {
  return get_type_name() + this->to_string();
}

[[nodiscard]] auto basic_object::get_type_name() const noexcept -> const
    char * {
  return abi::__cxa_demangle(get_type().name(), nullptr, nullptr, nullptr);
}
} // namespace viole
