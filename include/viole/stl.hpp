#pragma once
#include "viole/base.hpp"
namespace viole {
template <typename T> class basic_queue : public basic_object {
  void push(T &&);
  void push(const T &);
  [[nodiscard]] viole::string to_string() const noexcept override;
  [[nodiscard]] const std::type_info &get_type() const noexcept override;
};
} // namespace viole
