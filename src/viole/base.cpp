#include "viole/base.hpp"
namespace viole {
[[nodiscard]] auto
basic_object::get_type() const noexcept -> const std::type_info & {
  return typeid(basic_object);
}

auto basic_object::to_string(basic_output &output) const noexcept -> void {
  output.write_string(to_string());
}
[[nodiscard]] auto basic_object::to_string() const noexcept -> viole::string {
  return "[basic_object]";
}
} // namespace viole
