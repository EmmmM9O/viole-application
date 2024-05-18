#pragma once
#include <type_traits>
namespace viole {
template <typename Type>
concept enum_template = std::is_enum<Type>::value;

}
