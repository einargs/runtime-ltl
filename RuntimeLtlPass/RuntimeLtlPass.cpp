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
#include "llvm/Pass.h" #include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

#define DEBUG_TYPE "runtimeltl"

namespace {

struct RuntimeLtl : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  RuntimeLtl() : FunctionPass(ID) {}

  Function *EntryFn;
  Function *ExitFn;

  void loadExternalFunctions(Module &module) {
    LLVMContext &ctx = module.getContext();
    EntryFn = dyn_cast<Function>(module.getOrInsertFunction("runtime_ltl_entry_fn", Type::getVoidTy(ctx)));
    ExitFn = dyn_cast<Function>(module.getOrInsertFunction("runtime_ltl_exit_fn", Type::getVoidTy(ctx)));
  }

  // Move annotations from global metadata to attributes on the functions
  // themselves.
  // Source code adapted from: http://bholt.org/posts/llvm-quick-tricks.html
  // retrieved March 4, 2019.
  void loadAnnotations(Module &module) {
    auto global_annos = module.getNamedGlobal("llvm.global.annotations");

    if (global_annos) {
      auto a = cast<ConstantArray>(global_annos->getOperand(0));
      for (int i=0; i<a->getNumOperands(); i++) {
        auto e = cast<ConstantStruct>(a->getOperand(i));

        if (Function* fn = dyn_cast<Function>(e->getOperand(0)->getOperand(0))) {
          auto anno = cast<ConstantDataArray>(cast<GlobalVariable>(e->getOperand(1)->getOperand(0))->getOperand(0))->getAsCString();

          // Log function name
          errs() << "annotated function ";
          errs().write_escaped(fn->getName()) << " with attribute " << anno
            << "\n";
          // Add annotation
          fn->addFnAttr(anno);
        }
      }
    }
  }

  bool doInitialization(Module &module) {
    loadAnnotations(module);
    loadExternalFunctions(module);

    if (!EntryFn || !ExitFn) {
      errs() << "Could not find verification functions\n";
    }

    return false;
  }

  bool runOnFunction(Function &F) override {
    //TODO: figure out how to pass clang attributes through.
    if (!F.hasFnAttribute("ltl_verify")) {
      return false;
    }

    // Create IRBuilder
    IRBuilder<> builder(&F.front());

    // Move to before the first instruction in the first basic block
    builder.SetInsertPoint(&F.front().front());
    builder.CreateCall(EntryFn);

    // Move to after the last instruction in the last basic block
    builder.SetInsertPoint(&F.back().back());
    builder.CreateCall(ExitFn);

    errs() << "Inserting calls in function";
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
static RegisterPass<RuntimeLtl> X("runtimeltl", "Runtime LTL Pass");
