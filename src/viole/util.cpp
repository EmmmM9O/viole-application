#pragma once
#include "viole/util.hpp"
#include <ostream>
namespace viole {

auto operator<<(std::ostream &stream,
                const basic_object &object) -> std::ostream & {
  stream << object.to_string();
  return stream;
}

} // namespace viole
