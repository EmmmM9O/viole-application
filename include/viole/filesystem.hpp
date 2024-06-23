/**
 * @file
 * @brief [TODO:description]
 */

#pragma once
#include "viole/base.hpp"
#include "viole/stl.hpp"
#include <exception>
#include <filesystem>
namespace viole {
enum class fi_types { directory = 2, file = 1, none = 0 };

/**
 * @class fi_runtime_error
 * @brief [TODO:description]
 *
 */
class fi_runtime_error : public basic_object, public std::exception {
private:
  std::filesystem::path m_path;
  viole::string m_error;
  uint m_code;

public:
  [[nodiscard]] auto get_code() const noexcept -> uint;
  [[nodiscard]] auto get_path() const noexcept -> std::filesystem::path;
  fi_runtime_error(std::filesystem::path &&path, viole::string &&error,
                   uint code);
  fi_runtime_error(const std::filesystem::path &path, viole::string &&error,
                   uint code);
  [[viole::must_override_del]] [[nodiscard]] auto
  to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;

  [[nodiscard]] auto what() const noexcept -> const char * override;
  ~fi_runtime_error() noexcept override = default;
};
/**
 * @class fi_not_exists
 * @brief [TODO:description]
 *
 */
class fi_not_exists : public fi_runtime_error {
public:
  explicit fi_not_exists(std::filesystem::path &&path);
  explicit fi_not_exists(const std::filesystem::path &path);
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
};
class fi_type_error : public fi_runtime_error {
private:
  fi_types m_type;

public:
  explicit fi_type_error(std::filesystem::path &&path, fi_types type);
  explicit fi_type_error(const std::filesystem::path &path, fi_types type);
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
  [[nodiscard]] auto get_fi_type() const noexcept -> fi_types;
};
/**
 * @class fi
 * @brief [TODO:description]
 *
 */
class fi : public viole::basic_object {
private:
  std::filesystem::path m_path;
  fi_types m_type = fi_types::none;

public:
  explicit fi() = delete;
  explicit fi(const char *path);
  explicit fi(const viole::string &path);
  explicit fi(std::filesystem::path path);
  fi(const fi &);
  fi(fi &);
  fi &operator=(const fi &);
  fi &operator=(fi &&) noexcept;
  fi(fi &&) noexcept;
  ~fi() override = default;
  auto sync_fi_type() noexcept -> fi_types;
  [[nodiscard]] auto get_fi_type() const noexcept -> fi_types;

  [[nodiscard]] auto exists() const noexcept -> bool;
  [[nodiscard]] auto is_directory() const noexcept -> bool;
  [[nodiscard]] auto is_file() const noexcept -> bool;

  [[nodiscard]] auto file_name() const noexcept -> viole::string;
  [[nodiscard]] auto absolute_path() const noexcept -> viole::string;
  [[nodiscard]] auto path() const noexcept -> viole::string;
  [[nodiscard]] auto read_string() const -> viole::string;

  [[nodiscard]] auto file_size() const -> uintmax_t;

  [[nodiscard]] auto list() const -> viole::vector<fi>;

  auto mkdir() -> void;
  auto touch() -> void;
  auto rename(const viole::string &) -> void;
  auto move(const viole::string &) -> void;
  auto move(viole::fi &) -> void;
  auto copy(viole::fi &) const -> void;
  auto remove() -> void;

  auto write_string(const viole::string &) const -> void;

  auto operator/(const char *child) -> fi;
  auto operator+(const char *child) -> fi;
  auto operator/=(const char *child) -> fi &;
  auto operator+=(const char *child) -> fi &;
  auto operator--() -> fi;
  auto operator--(int) -> fi;
  auto operator==(const fi &other) -> bool;
  [[nodiscard]] auto parent() const -> fi;

  [[nodiscard]] auto to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
};
} // namespace viole
