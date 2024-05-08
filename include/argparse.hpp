#pragma once
#include <type_traits>
namespace viole {
template <typename T>
concept Enum = std::is_enum<T>::value;
}
