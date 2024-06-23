#include "viole-plugin/consumer.hpp"
#include "viole-plugin/base.hpp"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include <clang/AST/Attrs.inc>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/Type.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <llvm/Support/Casting.h>
namespace viole_plugin {
auto decl_must_override_callback::run(
    const clang::ast_matchers::MatchFinder::MatchResult &result) -> void {
  auto *context = result.Context;
  auto &m_diags = context->getDiagnostics();
  m_warning_not_override = m_diags.getCustomDiagID(
      error? clang::DiagnosticsEngine::Error : clang::DiagnosticsEngine::Warning ,
      "virtual function %q0 is marked as 'must_override' but not override "
      "method does not override the base");
  m_note_previous_must_override = m_diags.getCustomDiagID(
      clang::DiagnosticsEngine::Note, "function marked 'must_override' here");
  const auto class_decl =
      result.Nodes.getNodeAs<clang::CXXRecordDecl>("classdef");

  llvm::SmallPtrSet<const clang::CXXMethodDecl *, 16> base_funcs;
  for (auto &base : class_decl->bases()) {
    auto type = base.getType();
    if (const auto *base_record_type = type->getAs<clang::RecordType>()) {
      if (const auto *base_decl =
              base_record_type->getDecl()->getDefinition()) {
        for (const auto &ite : base_decl->decls()) {
          if (auto *func = llvm::dyn_cast<clang::CXXMethodDecl>(ite)) {
            if (is_marked_as_must_override(func)) {
              base_funcs.insert(func);
            }
          }
        }
      }
    }
  }
  for (const auto &func : class_decl->methods()) {
    for (const auto &tog : base_funcs) {
      if (tog->getNameAsString() == func->getNameAsString()) {
        must_override_methods.insert(func);
        base_funcs.erase(tog);
      }
    }
  }
  if (!base_funcs.empty()) {
    for (const auto &tog : base_funcs) {
      m_diags.Report(class_decl->getLocation(), m_warning_not_override)
          << tog << class_decl;
      m_diags.Report(tog->getLocation(), m_note_previous_must_override);
    }
  }
}

auto viole_plugin_consumer::HandleTranslationUnit(clang::ASTContext &context)
    -> void {
  auto &diags = context.getDiagnostics();
  for (const auto *method : must_override_methods) {
    late_diag_appertains_to_decl(diags, method);
  }
  m_finder.matchAST(context);
}
viole_plugin_consumer::viole_plugin_consumer() {
  using namespace clang::ast_matchers;
  DeclarationMatcher decl_macher =
      cxxRecordDecl(isClass(), isDefinition(), isDerivedFrom(anything()))
          .bind("classdef");
  m_finder.addMatcher(decl_macher, &m_callback);
}
auto viole_plugin_consumer::late_diag_appertains_to_decl(
    clang::DiagnosticsEngine &diags,
    const clang::CXXMethodDecl *method_decl) -> void {
  if (method_decl->hasAttr<clang::FinalAttr>()) {
    unsigned m_id = diags.getCustomDiagID(
        clang::DiagnosticsEngine::Warning,
        "'must_override' attribute marked on a final method");
    diags.Report(method_decl->getLocation(), m_id);
  }
}
} // namespace viole_plugin
