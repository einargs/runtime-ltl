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

#include <string>
#include <vector>
#include <fstream>
#include <cxxabi.h>
#include <memory>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "./ConfigParser.h"

using namespace llvm; 

#define DEBUG_TYPE "runtimeltl"

// For information on defining command line arguments in the LLVM system,
// see: http://llvm.org/docs/CommandLine.html#quick-start-guide
// and: http://llvm.org/doxygen/LoopUnrollPass_8cpp_source.html
cl::opt<std::string> RuntimeLtlConfigFilename("runtime-ltl-config",
    cl::Required,
    cl::desc("Specify configuration file for runtime ltl verification system"),
    cl::value_desc("filename"));

// Call `ExitOnErr` to panic if an `Expected<T>` is an error instead of a value.
ExitOnError ExitOnErr;

namespace {
using config::parse;
using config::InstrumentationTarget;

// Function for demangling LLVM IR names.
//
// Will likely be used for checking if a function is an instance of a templated
// function.
Optional<std::string> demangle(StringRef ref) {
  size_t length = 0;
  int status;
  char *demangledNameRaw =
    abi::__cxa_demangle(ref.data(), NULL, &length, &status);

  switch (status) {
    case 0: {// Success
      std::string demangledNameStr(demangledNameRaw);
      free(demangledNameRaw);

      return demangledNameStr;
    }
    case -1: {// A memory allocation failure occured
      errs() << "ERROR " << status << " using abi::__cxa_demangle: memory allocation failure occured\n";
      return Optional<std::string>();
    }
    case -2: {// mangled_name is not a valid name under the C++ ABI mangling rules.
      return Optional<std::string>();
    }
    case -3: {// One of the arguments is invalid.
      errs() << "ERROR " << status << " using abi::__cxa_demangle: one of the arguments is invalid\n";
      return Optional<std::string>();
    }
  }
}

struct RuntimeLtl : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  RuntimeLtl() : FunctionPass(ID) {}

  RuntimeLtl(std::vector<InstrumentationTarget> &&targets)
    : FunctionPass(ID), targets(std::move(targets)) {}

  //TODO: consider moving these to a lookup table I can hit during the function
  // pass.
  //
  // Doing this would move annotating the functions from an O(m x n) operation
  // to an O(n) operation.
  const std::vector<InstrumentationTarget> targets;

  //TODO: these should probably be moved out of here, since I think that there's
  // only ever one RuntimeLtl object in existence? Check on that--it has
  // implications on how I deal with the vector of instrumentation targets too.
  Function *EntryFn;
  Function *ExitFn;

  void loadExternalFunctions(Module &module) {
    LLVMContext &ctx = module.getContext();
    EntryFn = dyn_cast<Function>(module.getOrInsertFunction("runtime_ltl_entry_fn", Type::getVoidTy(ctx)));
    ExitFn = dyn_cast<Function>(module.getOrInsertFunction("runtime_ltl_exit_fn", Type::getVoidTy(ctx)));
  }

  void loadConfigFile(Module &module) {
    errs() << "instrumentation targets:" << '\n';
    for (const InstrumentationTarget &target : targets) {
      if (Function *fn = module.getFunction(target.mangled_name)) {
        errs().write_escaped(target.mangled_name) << '\n';
        fn->addFnAttr("ltl_verify");
      }
    }
    errs() << '\n';
  }

  bool doInitialization(Module &module) {
    loadConfigFile(module);
    loadExternalFunctions(module);

    if (!EntryFn || !ExitFn) {
      errs() << "Could not find verification functions\n";
    }

    return false;
  }

  bool runOnFunction(Function &F) override {
    Optional<std::string> demangledName = demangle(F.getName());
    if (demangledName.hasValue()) {
      errs() << "Mangled name: ";
      errs().write_escaped(F.getName()) << '\n';
      errs() << "Demangled name: ";
      errs().write_escaped(demangledName.getValue()) << '\n';
    }

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

    errs() << "Inserting calls in function ";
    errs().write_escaped(F.getName()) << '\n';

    return true;
  }
};

}

char RuntimeLtl::ID = 0;

/// This is used not only to register the pass needed to inject code,
/// but it also is in charge of running the code to parse the configuration
/// file into a vector of instrumentation targets.
static void registerRuntimeLtl(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  auto targets_ptr = ExitOnErr(parse(RuntimeLtlConfigFilename));

  PM.add(new RuntimeLtl(std::move(*targets_ptr)));
}

static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerRuntimeLtl);
static RegisterPass<RuntimeLtl> X("runtimeltl", "Runtime LTL Pass");
