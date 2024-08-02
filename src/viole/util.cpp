#pragma once
#include "viole/util.hpp"
#include <ostream>
namespace viole {

auto operator<<(std::ostream &stream,
                const basic_object &object) -> std::ostream & {
  stream << object.to_string_full();
  return stream;
}
auto abi_type_info_to_string(const std::type_info type) -> std::string{
  return abi::__cxa_demangle(type.name(), nullptr, nullptr, nullptr);
}
} // namespace viole
