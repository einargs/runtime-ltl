//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

#define DEBUG_TYPE "runtimeltl"

namespace {

struct RuntimeLtl : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  RuntimeLtl() : FunctionPass(ID) {}

  Function *LogEnterFn;
  Function *LogExitFn;

  bool doInitialization(Module &module) {
    LogEnterFn = module.getFunction("log_enter");
    LogExitFn = module.getFunction("log_exit");

    if (!LogEnterFn || !LogExitFn) {
      errs() << "Could not find log functions\n";
    }

    return false;
  }

  bool runOnFunction(Function &F) override {
    //TODO: figure out how to pass clang attributes through.
    if (F.getName() != "annotate_this"
        // This is the function we will insert calls to; thus we really
        // can't make any calls to it.
       ) {
      return false;
    }

    // Create IRBuilder
    IRBuilder<> builder(&F.front());

    // Move to before the first instruction in the first basic block
    builder.SetInsertPoint(&F.front().front());
    builder.CreateCall(LogEnterFn);

    // Move to after the last instruction in the last basic block
    builder.SetInsertPoint(&F.back().back());
    builder.CreateCall(LogExitFn);

    errs() << "Annotating function: ";
    errs().write_escaped(F.getName()) << '\n';

    return true;
  }
};

}

char RuntimeLtl::ID = 0;
static void registerRuntimeLtl(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new RuntimeLtl());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerRuntimeLtl);
//static RegisterPass<RuntimeLtl> X("runtimeltl", "Runtime LTL Pass");
