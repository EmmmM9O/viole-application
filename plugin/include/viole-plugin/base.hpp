#pragma once
#include <clang/AST/DeclCXX.h>
#include <llvm/ADT/SmallPtrSet.h>
namespace viole_plugin {
#define VIOLE_SIZE 32
extern bool error;
extern llvm::SmallPtrSet<const clang::CXXMethodDecl *, VIOLE_SIZE>
    must_override_methods;
extern llvm::SmallPtrSet<const clang::CXXMethodDecl *, VIOLE_SIZE>
    must_override_del_methods;
auto is_marked_as_must_override(const clang::CXXMethodDecl *decl) -> bool;
} // namespace viole_plugin
