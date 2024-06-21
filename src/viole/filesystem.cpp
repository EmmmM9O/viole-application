#include "viole/filesystem.hpp"
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
    : std::runtime_error(to_string()), m_path(path), m_error(error),
      m_code(code) {}
fi_runtime_error::fi_runtime_error(const std::filesystem::path &path,
                                   viole::string &&error, uint code)
    : std::runtime_error(to_string()), m_path(path), m_error(error),
      m_code(code) {}
[[nodiscard]] auto
fi_runtime_error::to_string() const noexcept -> viole::string {
  return "[" + viole::string(get_type().name()) + "][fi][" + m_path.string() +
         "][" + m_error + "]";
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
    : fi_runtime_error(path, "type error", 1 + static_cast<int>(type)),
      m_type(type) {}
fi_type_error::fi_type_error(const std::filesystem::path &path, fi_types type)
    : fi_runtime_error(path, "type error", 1 + static_cast<int>(type)),
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
}

auto fi::mkdir() const -> void {}
auto fi::touch() const -> void {}
auto fi::rename(const viole::string &) -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
}

auto fi::move(const viole::string &path) -> void { move(fi(path)); }
auto fi::move(const viole::fi &) -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
}
auto fi::copy(const viole::fi &) const -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
}
auto fi::remove() const -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
}

auto fi::write_string(const viole::string &) const -> void {
  if (!exists()) {
    throw fi_not_exists(m_path);
  }
}

[[nodiscard]] auto fi::to_string() const noexcept -> viole::string {
  return "[fi]{" + m_path.string() + "}";
}
[[nodiscard]] auto fi::get_type() const noexcept -> const std::type_info & {
  return typeid(fi);
}
} // namespace viole
