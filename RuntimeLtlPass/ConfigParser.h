#ifndef RUNTIME_LTL_CONFIG_PARSER_H
#define RUNTIME_LTL_CONFIG_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "llvm/Support/Error.h"
#include "llvm/ADT/StringMap.h"

#include "./Ast.h"

namespace config {

enum struct InstrumentationTargetKind {
  Function,
  Method,
  Constructor,
  Destructor
};

struct InstrumentationTarget {
  InstrumentationTargetKind kind;
  std::string event_name;
  std::string mangled_function_name;
};

llvm::Expected<std::vector<InstrumentationTarget>> hydrate(
    LtlConfig &&config);

llvm::Expected<llvm::StringMap<InstrumentationTarget>> parse(
    std::string &filename);

llvm::StringMap<InstrumentationTarget>
  map_by_mangled_names(std::vector<InstrumentationTarget> &&targets);

}

#endif
