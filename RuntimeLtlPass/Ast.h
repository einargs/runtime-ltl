#ifndef RUNTIME_LTL_AST_H
#define RUNTIME_LTL_AST_H

#include <vector>
#include <string>

#include <nlohmann/json.hpp>

namespace config {

struct LtlEvent {
  std::string event_name;
  std::string specifier;
  bool enabled;
};

struct LtlConfig {
  std::vector<std::string> includes;
  std::vector<LtlEvent> events;
};

LtlConfig build_config(nlohmann::json &json_obj);

}

#endif
