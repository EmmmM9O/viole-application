#pragma once

#include "viole-plugin/arrtibute.hpp"
#include "viole-plugin/consumer.hpp"

#include <clang/AST/ASTConsumer.h>
#include <clang/Basic/ParsedAttrInfo.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <llvm/ADT/StringRef.h>
#include <memory>
namespace viole_plugin {
class viole_plugin_action : public clang::PluginASTAction {

  auto CreateASTConsumer(clang::CompilerInstance &cci, llvm::StringRef)
      -> std::unique_ptr<clang::ASTConsumer> override {
    return std::make_unique<viole_plugin_consumer>();
  }

  auto ParseArgs(const clang::CompilerInstance & /*CI*/,
                 const std::vector<std::string> &args) -> bool override {
    if (!args.empty() && args[0] == "help") {
      llvm::errs() << "Help for the Viole Plugin\n";
    }
    if (!args.empty() && args[0] == "warn") {
      error = false;
    }
    if (!args.empty() && args[0] == "error") {
      error = true;
    }
    return true;
  }
  auto getActionType() -> clang::PluginASTAction::ActionType override {
    return clang::PluginASTAction::ActionType::AddBeforeMainAction;
  }
  
};
} // namespace viole_plugin
// NOLINTBEGIN
static clang::FrontendPluginRegistry::Add<viole_plugin::viole_plugin_action>
    X("viole_plugin", "the custome plugin for the project viole-application");
static clang::ParsedAttrInfoRegistry::Add<viole_plugin::must_override_attr_info>
    Y("must_override_attr", "Attribute for viole plugin");
static clang::ParsedAttrInfoRegistry::Add<
    viole_plugin::must_override_del_attr_info>
    Y2("must_override_del_attr", "Attribute for viole plugin");
// NOLINTEND
