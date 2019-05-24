#include "./Ast.h"

using json = nlohmann::json;

namespace {
using config::LtlEvent;

LtlEvent build_event(const std::string &name, json &json_event) {
  LtlEvent event;

  event.event_name = name;

  event.specifier = json_event["specifier"];
  event.enabled = json_event["enabled"];

  return event;
}

}

namespace config {

LtlConfig build_config(json &json_obj) {
  LtlConfig config;

  for (auto &include : json_obj["includes"]) {
    config.includes.push_back(include);
  }

  for (auto &j : json_obj["events"].items()) {
    config.events.push_back(build_event(j.key(), j.value()));
  }

  return config;
}

}
