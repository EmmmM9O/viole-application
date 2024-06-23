#pragma once
#include <clang/AST/DeclCXX.h>
#include <llvm/ADT/SmallPtrSet.h>
namespace viole_plugin {
extern bool error;
extern llvm::SmallPtrSet<const clang::CXXMethodDecl *, 16>
    must_override_methods;

auto is_marked_as_must_override(const clang::CXXMethodDecl *decl) -> bool;
} // namespace viole_plugin
