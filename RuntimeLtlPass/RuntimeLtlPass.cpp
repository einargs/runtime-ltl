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
#include <unordered_map>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/StringMap.h"
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

  RuntimeLtl(StringMap<InstrumentationTarget> &&targets)
    : FunctionPass(ID), target_map(std::move(targets)) {}

  const StringMap<InstrumentationTarget> target_map;

  //TODO: these should probably be moved out of here, since I think that there's
  // only ever one RuntimeLtl object in existence? Check on that--it has
  // implications on how I deal with the vector of instrumentation targets too.
  // UPDATE (Fri May 24 2019): I'm not even sure what this todo means.
  Function *EntryFn;
  Function *ExitFn;

  void insertRuntimeCall(
      IRBuilder<> &builder,
      Function *callee,
      const InstrumentationTarget &target) {
    GlobalVariable *event_name = builder.CreateGlobalString(target.event_name);
    const std::vector<Value *> args { event_name };
    builder.CreateCall(callee, args);
  }

  void loadExternalFunctions(Module &module) {
    LLVMContext &ctx = module.getContext();
    EntryFn = dyn_cast<Function>(module.getOrInsertFunction("runtime_ltl_entry_fn", Type::getVoidTy(ctx)));
    ExitFn = dyn_cast<Function>(module.getOrInsertFunction("runtime_ltl_exit_fn", Type::getVoidTy(ctx)));
  }

  bool doInitialization(Module &module) {
    loadExternalFunctions(module);

    if (!EntryFn || !ExitFn) {
      errs() << "Could not find verification functions\n";
    }

    //TODO: figure out what this return false means
    return false;
  }

  // 
  Optional<InstrumentationTarget> getInstrumentationTarget(Function &F) const {
    auto iter = target_map.find(F.getName());
    if (iter == target_map.end()) {
      return Optional<InstrumentationTarget>();
    } else {
      const InstrumentationTarget target = iter->getValue();
      return Optional<InstrumentationTarget>(std::move(target));
    }
  }

  bool runOnFunction(Function &F) override {
    Optional<std::string> demangledName = demangle(F.getName());

    if (demangledName.hasValue()) {
      errs() << "Mangled name: ";
      errs().write_escaped(F.getName()) << '\n';
      errs() << "Demangled name: ";
      errs().write_escaped(demangledName.getValue()) << '\n';
    }

    auto optional_target = getInstrumentationTarget(F);
    if (!optional_target.hasValue()) {
      return false;
    }
    const InstrumentationTarget &target = *optional_target;

    // Create IRBuilder
    IRBuilder<> builder(&F.front());

    // Move to before the first instruction in the first basic block
    builder.SetInsertPoint(&F.front().front());
    insertRuntimeCall(builder, EntryFn, target);

    // Move to after the last instruction in the last basic block
    builder.SetInsertPoint(&F.back().back());
    insertRuntimeCall(builder, ExitFn, target);

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
  auto targets = ExitOnErr(config::parse(RuntimeLtlConfigFilename));

  PM.add(new RuntimeLtl(std::move(targets)));
}

static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerRuntimeLtl);
static RegisterPass<RuntimeLtl> X("runtimeltl", "Runtime LTL Pass");
