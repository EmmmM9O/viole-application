module;
#include <any>
#include <typeinfo>
#include <utility>
module viole_util;
const char *viole::bad_any_point::what() const noexcept {
  return "bad_any_ref exception";
}
[[nodiscard]] bool viole::any_point::has_value() const {
  return _data.get() != nullptr;
}

[[nodiscard]] const std::type_info &viole::any_point::type() const {
  return _data->get_type();
}
bool viole::any_point::same_as(const std::any &data) {
  return type() == data.type();
}
void viole::any_point::reset() noexcept { _data.reset(); }
void viole::any_point::set_any(std::any data) {
  if (same_as(data))
    _data->set_any(data);
  else
    throw bad_any_point();
}
viole::any_point &viole::any_point::operator=(const any_point &other) {
  if (type() != other.type()) {
    throw bad_any_point();
  }
  if (other._data) {
    _data = other._data;
  }
  return *this;
}
viole::any_point::any_point(const any_point &other) {

  if (other._data) {
    _data = other._data;
  }
}
viole::any_point::any_point(any_point &&other) noexcept
    : _data(std::move(other._data)) {}
viole::any_point &viole::any_point::operator=(any_point &&other) noexcept {
  if (type() != other.type()) {
    throw bad_any_point();
  }
  _data.swap(other._data);
  return *this;
}
void viole::any_point::swap(any_point &other) { _data.swap(other._data); }
