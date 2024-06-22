#include "viole/filesystem.hpp"
#include "viole/util.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
namespace viole {

[[nodiscard]] auto fi_runtime_error::get_code() const noexcept -> uint {
  return m_code;
}
[[nodiscard]] auto
fi_runtime_error::get_path() const noexcept -> std::filesystem::path {
  return m_path;
}
fi_runtime_error::fi_runtime_error(std::filesystem::path &&path,
                                   viole::string &&error, uint code)
    : m_path(path), m_error(error), m_code(code) {}
fi_runtime_error::fi_runtime_error(const std::filesystem::path &path,
                                   viole::string &&error, uint code)
    : m_path(path), m_error(error), m_code(code) {}
[[nodiscard]] auto
fi_runtime_error::to_string() const noexcept -> viole::string {
  return "[fi]{[" + m_path.string() + "][" + m_error + "]}";
}

[[nodiscard]] auto fi_runtime_error::what() const noexcept -> const char * {
  std::runtime_error tmp{to_string_full()};
  return tmp.what();
}
[[nodiscard]] auto
fi_runtime_error::get_type() const noexcept -> const std::type_info & {
  return typeid(fi_runtime_error);
}

fi_not_exists::fi_not_exists(std::filesystem::path &&path)
    : fi_runtime_error(path, "not exists", 0) {}
fi_not_exists::fi_not_exists(const std::filesystem::path &path)
    : fi_runtime_error(path, "not exists", 0) {}
[[nodiscard]] auto
fi_not_exists::get_type() const noexcept -> const std::type_info & {
  return typeid(fi_not_exists);
}

fi_type_error::fi_type_error(std::filesystem::path &&path, fi_types type)
    : fi_runtime_error(path,
                       string("type error") + " need " + enum_to_string(type),
                       1 + static_cast<int>(type)),
      m_type(type) {}
fi_type_error::fi_type_error(const std::filesystem::path &path, fi_types type)
    : fi_runtime_error(path,
                       string("type error") + " need " + enum_to_string(type),
                       1 + static_cast<int>(type)),
      m_type(type) {}
[[nodiscard]] auto
fi_type_error::get_type() const noexcept -> const std::type_info & {
  return typeid(fi_type_error);
}

[[nodiscard]] auto fi_type_error::get_fi_type() const noexcept -> fi_types {
  return m_type;
}

auto fi::sync_fi_type() noexcept -> fi_types {
  if (!exists()) {
    m_type = fi_types::none;
    return m_type;
  }
  switch (std::filesystem::directory_entry(m_path).status().type()) {
    using type = std::filesystem::file_type;
  case type::directory:
    m_type = fi_types::directory;
    break;
  case type::regular:
    m_type = fi_types::file;
    break;
  default:
    m_type = fi_types::none;
  }
  return m_type;
}
[[nodiscard]] auto fi::get_fi_type() const noexcept -> fi_types {
  return m_type;
}

fi::fi(const char *path) : m_path(path) {}
fi::fi(const viole::string &path) : m_path(path) { sync_fi_type(); }
fi::fi(const fi &file) : m_path(file.m_path) { sync_fi_type(); }
fi::fi(fi &file) : m_path(file.m_path) { sync_fi_type(); }
fi &fi::operator=(const fi &file) {
  if (this != &file) {
    sync_fi_type();
    m_path = file.m_path;
  }
  return *this;
}
fi &fi::operator=(fi &&path) noexcept {

  sync_fi_type();
  m_path = path.m_path;
  return *this;
}
fi::fi(fi &&path) noexcept : m_path(std::move(path.m_path)) { sync_fi_type(); }

fi::fi(std::filesystem::path path) : m_path(std::move(path)) { sync_fi_type(); }
[[nodiscard]] auto fi::exists() const noexcept -> bool {
  return std::filesystem::exists(m_path);
}
[[nodiscard]] auto fi::is_directory() const noexcept -> bool {
  return m_type == fi_types::directory;
}
[[nodiscard]] auto fi::is_file() const noexcept -> bool {
  return m_type == fi_types::file;
}

[[nodiscard]] auto fi::file_name() const noexcept -> viole::string {
  return m_path.filename().string();
}
[[nodiscard]] auto fi::absolute_path() const noexcept -> viole::string {
  return std::filesystem::absolute(m_path).string();
}
[[nodiscard]] auto fi::path() const noexcept -> viole::string {
  return m_path.string();
}
[[nodiscard]] auto fi::read_string() const -> viole::string {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  std::ifstream instream(m_path.string());
  std::ostringstream tmp;
  tmp << instream.rdbuf();
  return tmp.str();
}

[[nodiscard]] auto fi::file_size() const -> uintmax_t {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  return std::filesystem::file_size(m_path);
}

[[nodiscard]] auto fi::list() const -> viole::vector<fi> {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  if (!is_directory()) {
    throw fi_type_error(m_path, fi_types::directory);
  }
  vector<fi> list;
  std::filesystem::directory_iterator dir(m_path);
#pragma unroll 5
  for (const auto &ite : dir) {
    list.emplace_back(ite);
  }
  return list;
}

auto fi::mkdir() -> void {
  std::filesystem::create_directories(m_path);
  sync_fi_type();
}
auto fi::touch() -> void {
  std::ofstream ofs{m_path};
  ofs.close();
  sync_fi_type();
}
auto fi::rename(const viole::string &new_name) -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  std::filesystem::rename(m_path, m_path.parent_path().concat(new_name));
}

auto fi::move(const viole::string &path) -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  std::filesystem::rename(m_path, std::filesystem::path(path));
}
auto fi::move(viole::fi &path) -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  std::filesystem::rename(m_path, path.m_path);
  path.sync_fi_type();
}
auto fi::copy(viole::fi &path) const -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  std::filesystem::copy(m_path, path.m_path);
  path.sync_fi_type();
}
auto fi::remove() -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  std::filesystem::remove(m_path);
  sync_fi_type();
}

auto fi::write_string(const viole::string &context) const -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
  if (!is_file()) {
    throw fi_type_error(m_path, fi_types::file);
  }
  std::ofstream ofs{m_path};
  ofs << context;
  ofs.close();
}

auto fi::operator/(const char *child) -> fi { return fi{m_path / child}; }
auto fi::operator+(const char *child) -> fi { return fi{m_path.concat(child)}; }
auto fi::operator/=(const char *child) -> fi & {
  m_path /= child;
  sync_fi_type();
  return *this;
}
auto fi::operator+=(const char *child) -> fi & {
  m_path += child;
  return *this;
}
auto fi::operator--() -> fi { return fi{m_path.parent_path()}; }
auto fi::operator--(int) -> fi {
  fi tmp{*this};
  m_path = m_path.parent_path();
  sync_fi_type();
  return tmp;
}
auto fi::operator==(const fi &other) -> bool {
  return this->m_path == other.m_path;
}

[[nodiscard]] auto fi::parent() const -> fi { return fi{m_path.parent_path()}; }

[[nodiscard]] auto fi::to_string() const noexcept -> viole::string {
  return "{" + m_path.string() + "}";
}
[[nodiscard]] auto fi::get_type() const noexcept -> const std::type_info & {
  return typeid(fi);
}
} // namespace viole
