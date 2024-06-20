#pragma once
#include "viole/base.hpp"
#include "viole/stl.hpp"
#include <filesystem>
namespace viole {

class fi : public viole::basic_object {
private:
  std::filesystem::path m_path;

public:
  fi();
  explicit fi(const viole::string &path);
  fi(const fi &);
  fi(fi &);
  fi &operator=(const fi &);
  fi &operator=(fi &&) noexcept;
  fi(fi &&) noexcept;
  ~fi();

  [[nodiscard]] auto exits() const -> bool;
  [[nodiscard]] auto is_directory() const -> bool;
  [[nodiscard]] auto is_file() const -> bool;

  [[nodiscard]] auto file_name() const -> viole::string;
  [[nodiscard]] auto absolute_path() const -> viole::string;
  [[nodiscard]] auto path() const -> viole::string;
  [[nodiscard]] auto file_size() const -> uintmax_t;

  auto mkdir() const -> void;
  auto touch() const -> void;
  auto rename(const viole::string &) -> void;
  auto remove() -> void;

  auto write_string(const viole::string &) -> void;

  [[nodiscard]] auto to_string() const noexcept -> viole::string override;
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override;
};
} // namespace viole
