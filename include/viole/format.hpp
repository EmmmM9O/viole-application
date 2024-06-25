/**
 * @file format
 * @brief format
 * @author novarc
 * @date 2024-6-25
 */
/*
 * format("xxx${}xx${name}")
 * .add_env(global_format_env)
 * .add_args(1,4,………)
 * .add_option(
 * color|file
 * )
 * .format()
 * .print();
 */
#pragma once
namespace viole {
template <typename String> class basic_format_context {
public:
  explicit basic_format_context(String &&string) {}
};
} // namespace viole
