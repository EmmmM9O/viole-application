#pragma once
#include <clang/AST/DeclBase.h>
#include <clang/Basic/ParsedAttrInfo.h>
#include <clang/Sema/ParsedAttr.h>
namespace viole_plugin {

struct must_override_attr_info : public clang::ParsedAttrInfo {
  must_override_attr_info();
  auto diagAppertainsToDecl(clang::Sema &sema, const clang::ParsedAttr &attr,
                            const clang::Decl *decl) const -> bool override;
  auto handleDeclAttribute(clang::Sema &sema, clang::Decl *decl,
                           const clang::ParsedAttr &attr) const
      -> AttrHandling override;
};

struct must_override_del_attr_info : public clang::ParsedAttrInfo {
  must_override_del_attr_info();
  auto diagAppertainsToDecl(clang::Sema &sema, const clang::ParsedAttr &attr,
                            const clang::Decl *decl) const -> bool override;
  auto handleDeclAttribute(clang::Sema &sema, clang::Decl *decl,
                           const clang::ParsedAttr &attr) const
      -> AttrHandling override;
};
} // namespace viole_plugin
