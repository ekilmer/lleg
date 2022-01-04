/**
 * Copied from
 * https://github.com/banach-space/llvm-tutor/blob/90be75044070b6ae70d01c783504e763693cbdbe/lib/StaticCallCounter.cpp
 *
 * License: MIT
 */
#include "lleg/lleg.hpp"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

// Pretty-prints the result of this analysis
static void printStaticCCResult(llvm::raw_ostream& OutS,
                                const ResultStaticCC& DirectCalls);

//------------------------------------------------------------------------------
// StaticCallCounter Implementation
//------------------------------------------------------------------------------
auto StaticCallCounter::runOnModule(llvm::Module& M)
    -> StaticCallCounter::Result
{
  llvm::MapVector<const llvm::Function*, unsigned> Res;

  for (auto& Func : M) {
    for (auto& BB : Func) {
      for (auto& Ins : BB) {
        // If this is a call instruction then CB will be not null.
        auto* CB = llvm::dyn_cast<llvm::CallBase>(&Ins);
        if (nullptr == CB) {
          continue;
        }

        // If CB is a direct function call then DirectInvoc will be not null.
        auto* DirectInvoc = CB->getCalledFunction();
        if (nullptr == DirectInvoc) {
          continue;
        }

        // We have a direct function call - update the count for the function
        // being called.
        auto CallCount = Res.find(DirectInvoc);
        if (Res.end() == CallCount) {
          CallCount = Res.insert(std::make_pair(DirectInvoc, 0)).first;
        }
        ++CallCount->second;
      }
    }
  }

  return Res;
}

auto StaticCallCounterPrinter::run(llvm::Module& M,
                                   llvm::ModuleAnalysisManager& MAM)
    -> llvm::PreservedAnalyses
{
  auto DirectCalls = MAM.getResult<StaticCallCounter>(M);

  printStaticCCResult(OS, DirectCalls);
  return llvm::PreservedAnalyses::all();
}

auto StaticCallCounter::run(llvm::Module& M,
                            llvm::ModuleAnalysisManager& /*unused*/)
    -> StaticCallCounter::Result
{
  return runOnModule(M);
}

auto LegacyStaticCallCounter::runOnModule(llvm::Module& M) -> bool
{
  DirectCalls = Impl.runOnModule(M);
  return false;
}

void LegacyStaticCallCounter::print(llvm::raw_ostream& OutS,
                                    llvm::Module const* /*unused*/) const
{
  printStaticCCResult(OutS, DirectCalls);
}

//------------------------------------------------------------------------------
// New PM Registration
//------------------------------------------------------------------------------
llvm::AnalysisKey StaticCallCounter::Key;

auto getStaticCallCounterPluginInfo() -> llvm::PassPluginLibraryInfo
{
  return {LLVM_PLUGIN_API_VERSION,
          "static-cc",
          LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB)
          {
            // #1 REGISTRATION FOR "opt -passes=print<static-cc>"
            PB.registerPipelineParsingCallback(
                [&](llvm::StringRef Name,
                    llvm::ModulePassManager& MPM,
                    llvm::ArrayRef<llvm::PassBuilder::PipelineElement>)
                {
                  if (Name == "print<static-cc>") {
                    MPM.addPass(StaticCallCounterPrinter(llvm::errs()));
                    return true;
                  }
                  return false;
                });
            // #2 REGISTRATION FOR "MAM.getResult<StaticCallCounter>(Module)"
            PB.registerAnalysisRegistrationCallback(
                [](llvm::ModuleAnalysisManager& MAM)
                { MAM.registerPass([&] { return StaticCallCounter(); }); });
          }};
}

//------------------------------------------------------------------------------
// Legacy PM Registration
//------------------------------------------------------------------------------
char LegacyStaticCallCounter::ID = 0;

// #1 REGISTRATION FOR "opt -analyze -legacy-static-cc"
llvm::RegisterPass<LegacyStaticCallCounter> X(
    /*PassArg=*/"legacy-static-cc",
    /*Name=*/"LegacyStaticCallCounter",
    /*CFGOnly=*/true,
    /*is_analysis=*/true);

//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------
static void printStaticCCResult(llvm::raw_ostream& OutS,
                                const ResultStaticCC& DirectCalls)
{
  OutS << "================================================="
       << "\n";
  OutS << "LLVM-TUTOR: static analysis results\n";
  OutS << "=================================================\n";
  const char* Str1 = "NAME";
  const char* Str2 = "#N DIRECT CALLS";
  OutS << llvm::format("%-20s %-10s\n", Str1, Str2);
  OutS << "-------------------------------------------------"
       << "\n";

  for (const auto& CallCount : DirectCalls) {
    OutS << llvm::format("%-20s %-10lu\n",
                         CallCount.first->getName().str().c_str(),
                         CallCount.second);
  }

  OutS << "-------------------------------------------------"
       << "\n\n";
}
