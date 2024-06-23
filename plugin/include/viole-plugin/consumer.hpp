#pragma once
#include "viole-plugin/base.hpp"
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/Diagnostic.h>
namespace viole_plugin {
class decl_must_override_callback
    : public clang::ast_matchers::MatchFinder::MatchCallback {
private:
  unsigned m_warning_not_override;
  unsigned m_test_id;
  unsigned m_note_previous_must_override;

public:
  decl_must_override_callback() = default;
  auto run(const clang::ast_matchers::MatchFinder::MatchResult &result)
      -> void override;
};

class viole_plugin_consumer : public clang::ASTConsumer {
public:
  viole_plugin_consumer();
  auto HandleTranslationUnit(clang::ASTContext &context) -> void override;

private:
  clang::ast_matchers::MatchFinder m_finder;
  decl_must_override_callback m_callback;
  auto
  late_diag_appertains_to_decl(clang::DiagnosticsEngine &diags,
                               const clang::CXXMethodDecl *method_decl) -> void;
};
} // namespace viole_plugin
