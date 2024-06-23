#include "viole-plugin/arrtibute.hpp"
#include "viole-plugin/base.hpp"
#include <clang/AST/DeclCXX.h>
#include <clang/Basic/AttributeCommonInfo.h>
#include <clang/Basic/DiagnosticSema.h>
#include <clang/Basic/ParsedAttrInfo.h>
#include <clang/Sema/Sema.h>
#include <llvm/Support/Casting.h>
namespace viole_plugin {
must_override_attr_info::must_override_attr_info() {
  OptArgs = 0;

  static constexpr Spelling S[] = {
      {clang::ParsedAttr::AS_GNU, "must_override"},
      {clang::ParsedAttr::AS_CXX11, "viole::must_override"}};
  Spellings = S;
}
auto must_override_attr_info::diagAppertainsToDecl(
    clang::Sema &sema, const clang::ParsedAttr &attr,
    const clang::Decl *decl) const -> bool {
  const auto *the_method = dyn_cast_or_null<clang::CXXMethodDecl>(decl);
  if ((the_method == nullptr) || !the_method->isVirtual()) {
    sema.Diag(attr.getLoc(), clang::diag::warn_attribute_wrong_decl_type_str)
        << attr << attr.isRegularKeywordAttribute() << "virtual functions";
    return false;
  }
  must_override_methods.insert(the_method);
  return true;
}
static auto is_must_override(const clang::AttrVec &attrs) -> bool {
  for (const auto *attr : attrs) {
    if (attr->getSpelling() == "viole::must_override") {
      return true;
    }
  }
  return false;
}
auto must_override_attr_info::handleDeclAttribute(
    clang::Sema &sema, clang::Decl *decl,
    const clang::ParsedAttr &attr) const -> AttrHandling {
  return AttributeNotApplied;
}

must_override_del_attr_info::must_override_del_attr_info() {
  OptArgs = 0;

  static constexpr Spelling s[] = {
      {clang::ParsedAttr::AS_GNU, "must_override_del"},
      {clang::ParsedAttr::AS_CXX11, "viole::must_override_del"}};
  Spellings = s;
}
auto must_override_del_attr_info::diagAppertainsToDecl(
    clang::Sema &sema, const clang::ParsedAttr &attr,
    const clang::Decl *decl) const -> bool {
  const auto *the_method = dyn_cast_or_null<clang::CXXMethodDecl>(decl);
  if ((the_method == nullptr) || 
      is_must_override(the_method->getAttrs())) {
    sema.Diag(attr.getLoc(), clang::diag::warn_attribute_wrong_decl_type_str )
        << attr << attr.isRegularKeywordAttribute() << "functions without must_override";
    return false;
  }
  must_override_del_methods.insert(the_method);
  return true;
}

auto must_override_del_attr_info::handleDeclAttribute(
    clang::Sema &sema, clang::Decl *decl,
    const clang::ParsedAttr &attr) const -> AttrHandling {
  return AttributeNotApplied;
}
} // namespace viole_plugin
