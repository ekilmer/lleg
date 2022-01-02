#pragma once

#include "cmake-init-llvm-fork/cmake-init-llvm-fork_export.hpp"
#include "llvm/ADT/MapVector.h"
#include "llvm/IR/AbstractCallSite.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

/**
 * A note about the MSVC warning C4251:
 * This warning should be suppressed for private data members of the project's
 * exported classes, because there are too many ways to work around it and all
 * involve some kind of trade-off (increased code complexity requiring more
 * developer time, writing boilerplate code, longer compile times), but those
 * solutions are very situational and solve things in slightly different ways,
 * depending on the requirements of the project.
 * That is to say, there is no general solution.
 *
 * What can be done instead is understand where issues could arise where this
 * warning is spotting a legitimate bug. I will give the general description of
 * this warning's cause and break it down to make it trivial to understand.
 *
 * C4251 is emitted when an exported class has a non-static data member of a
 * non-exported class type.
 *
 * The exported class in our case is the class below (exported_class), which
 * has a non-static data member (m_name) of a non-exported class type
 * (std::string).
 *
 * The rationale here is that the user of the exported class could attempt to
 * access (directly, or via an inline member function) a static data member or
 * a non-inline member function of the data member, resulting in a linker
 * error.
 * Inline member function above means member functions that are defined (not
 * declared) in the class definition.
 *
 * Since this exported class never makes these non-exported types available to
 * the user, we can safely ignore this warning. It's fine if there are
 * non-exported class types as private member variables, because they are only
 * accessed by the members of the exported class itself.
 *
 * The name() method below returns a pointer to the stored null-terminated
 * string as a fundamental type (const char), so this is safe to use anywhere.
 * The only downside is that you can have dangling pointers if the pointer
 * outlives the class instance which stored the string.
 *
 * Shared libraries are not easy, they need some discipline to get right, but
 * they also solve some other problems that make them worth the time invested.
 */

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
using ResultStaticCC = llvm::MapVector<const llvm::Function*, unsigned>;

struct CMAKE_INIT_LLVM_FORK_EXPORT StaticCallCounter
    : public llvm::AnalysisInfoMixin<StaticCallCounter>
{
  using Result = ResultStaticCC;
  auto run(llvm::Module& M, llvm::ModuleAnalysisManager& /*unused*/) -> Result;
  auto runOnModule(llvm::Module& M) -> Result;
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static auto isRequired() -> bool
  {
    return true;
  }

private:
  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
  friend struct llvm::AnalysisInfoMixin<StaticCallCounter>;
};

//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
class CMAKE_INIT_LLVM_FORK_EXPORT StaticCallCounterPrinter
    : public llvm::PassInfoMixin<StaticCallCounterPrinter>
{
public:
  explicit StaticCallCounterPrinter(llvm::raw_ostream& OutS)
      : OS(OutS)
  {
  }
  auto run(llvm::Module& M, llvm::ModuleAnalysisManager& MAM)
      -> llvm::PreservedAnalyses;
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static auto isRequired() -> bool
  {
    return true;
  }

private:
  llvm::raw_ostream& OS;
};

CMAKE_INIT_LLVM_FORK_EXPORT auto getStaticCallCounterPluginInfo()
    -> llvm::PassPluginLibraryInfo;

// Windows has no concept of weak symbols
#ifndef _WIN32
extern "C" LLVM_ATTRIBUTE_WEAK CMAKE_INIT_LLVM_FORK_EXPORT auto
llvmGetPassPluginInfo() -> llvm::PassPluginLibraryInfo
{
  return getStaticCallCounterPluginInfo();
}
#endif // _WIN32

//------------------------------------------------------------------------------
// Legacy PM interface
//------------------------------------------------------------------------------
struct CMAKE_INIT_LLVM_FORK_EXPORT LegacyStaticCallCounter
    : public llvm::ModulePass
{
  static char ID;
  LegacyStaticCallCounter()
      : llvm::ModulePass(ID)
  {
  }
  auto runOnModule(llvm::Module& M) -> bool override;
  // The print method must be implemented by Legacy analysis passes in order to
  // print a human readable version of the analysis results:
  //    http://llvm.org/docs/WritingAnLLVMPass.html#the-print-method
  void print(llvm::raw_ostream& OutS,
             llvm::Module const* /*unused*/) const override;

  ResultStaticCC DirectCalls;
  StaticCallCounter Impl;
};
