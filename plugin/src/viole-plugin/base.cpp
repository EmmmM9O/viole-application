#include <viole-plugin/base.hpp>

namespace viole_plugin {
bool error=true;
llvm::SmallPtrSet<const clang::CXXMethodDecl *, 16> must_override_methods{};
auto is_marked_as_must_override(const clang::CXXMethodDecl *decl) -> bool {
  return must_override_methods.contains(decl);
}
} // namespace viole_plugin
