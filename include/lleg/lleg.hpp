//===- lleg/lleg.hpp - LLEG main header -------------------------*- C++ -*-===//
//
// Copied originally from
// https://github.com/banach-space/llvm-tutor/blob/90be75044070b6ae70d01c783504e763693cbdbe/include/StaticCallCounter.h
// SPDX-License-Identifier: MIT
//
//===----------------------------------------------------------------------===//
///
/// \file lleg.hpp
/// \brief Declares the StaticCallCounter Passes
///
/// The implementation includes:
///     * new pass manager interface
///     * legacy pass manager interface
///     * printer pass for the new pass manager
///
//===----------------------------------------------------------------------===//

#ifndef LLEG_LLEG_HPP
#define LLEG_LLEG_HPP

#include "lleg/lleg_export.hpp"
#include "llvm/ADT/MapVector.h"
#include "llvm/IR/AbstractCallSite.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace lleg {

//===-----------------------------------------------------------------------===/
// New PM interface
//===-----------------------------------------------------------------------===/

/// ResultStaticCC - Mapping of function to number of calls in that function.
using ResultStaticCC = llvm::MapVector<const llvm::Function *, unsigned>;

//===----------------------------------------------------------------------===//
/// New LLVM Pass Manager interface for counting calls in a module.
///
struct LLEG_EXPORT StaticCallCounter
    : public llvm::AnalysisInfoMixin<StaticCallCounter> {
  /// Result of our pass.
  using Result = ResultStaticCC;
  /// Run our call counter pass on the module.
  Result run(llvm::Module &M, llvm::ModuleAnalysisManager & /*unused*/);
  /// Run our call counter pass on the module.
  Result runOnModule(llvm::Module &M);
  /// Part of the official API:
  /// https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
  friend struct llvm::AnalysisInfoMixin<StaticCallCounter>;
};

//===----------------------------------------------------------------------===//
/// New LLVM Pass Manager interface for the printer pass for our call counter.
///
class LLEG_EXPORT StaticCallCounterPrinter
    : public llvm::PassInfoMixin<StaticCallCounterPrinter> {
public:
  /// Constructor takes output stream for where to print results.
  explicit StaticCallCounterPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
  /// Runs the print pass on the module.
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
  /// Part of the official API:
  /// https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
};

/// Return the plugin info for the call counter pass.
LLEG_EXPORT llvm::PassPluginLibraryInfo getStaticCallCounterPluginInfo();

// Windows has no concept of weak symbols
#ifndef _WIN32
/// Return the plugin info for this call counter pass.
extern "C" LLVM_ATTRIBUTE_WEAK LLEG_EXPORT llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo();
#endif // _WIN32

//===----------------------------------------------------------------------===//
// Legacy PM interface
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
/// Legacy LLVM Pass Manager Interface for counting calls in all functions in
/// the module.
///
struct LLEG_EXPORT LegacyStaticCallCounter : public llvm::ModulePass {
  /// ID of the pass.
  static char ID;
  LegacyStaticCallCounter() : llvm::ModulePass(ID) {}

  /// Runs the call counter pass on the module.
  bool runOnModule(llvm::Module &M) override;

  /// The print method must be implemented by Legacy analysis passes in order to
  /// print a human-readable version of the analysis results:
  /// http://llvm.org/docs/WritingAnLLVMPass.html#the-print-method
  void print(llvm::raw_ostream &OutS,
             llvm::Module const * /*unused*/) const override;

  /// Holds our results for calls in each function.
  ResultStaticCC DirectCalls;
  /// Implementation object for our pass logic.
  StaticCallCounter Impl;
};

} // namespace lleg

#endif // LLEG_LLEG_HPP
