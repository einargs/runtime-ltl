#ifndef RUNTIME_LTL_CONFIG_PARSER_H
#define RUNTIME_LTL_CONFIG_PARSER_H

#include <string>
#include <vector>
#include <memory>

#include "llvm/Support/Error.h"

namespace config {

enum struct InstrumentationTargetKind {
  Function,
  Method,
  Constructor,
  Destructor
};

struct InstrumentationTarget {
  InstrumentationTargetKind kind;
  std::string mangled_name;
};

llvm::Expected<std::unique_ptr<std::vector<InstrumentationTarget>>> parse(std::string config_file);

}

#endif
