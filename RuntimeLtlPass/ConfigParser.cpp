#include "./ConfigParser.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <system_error>

#include <clang-c/Index.h>  // This is libclang.

#include <nlohmann/json.hpp>

#include "./Ast.h"
#include "./Util.h" 
using std::ostream;
using std::ostringstream;
using std::ifstream;
using std::string;
using std::vector;
using std::unique_ptr;
using std::cout;
using std::cerr;
using std::endl;
using std::errc;
using std::unordered_map;

using llvm::Expected;
using llvm::make_unique;
using llvm::Optional;
using llvm::StringError;
using llvm::make_error;

using json = nlohmann::json;

using config::LtlEvent;
using config::LtlConfig;
using config::InstrumentationTarget;
using config::InstrumentationTargetKind;
using config::convertCXString;
using config::operator<<;

namespace {

//
CXUnsavedFile unsaved_file_from_string(
    char *filename, std::string &contents) {
  CXUnsavedFile file = {
    .Filename = filename,
    .Contents = contents.c_str(),
    .Length = contents.length()
  };

  return file;
}

std::string make_parsable_file(LtlConfig &config) {
  ostringstream file_body;

  // Prepare stream for output
  std::ostream::sentry sentry(file_body);

  for (auto &include : config.includes) {
    file_body << "#include \"" << include << "\"\n";
  }

  file_body << "\n";

  for (auto &event : config.events) {
    file_body << event.specifier << ";\n";
  }
  
  return file_body.str();
}

// Attempt to get an InstrumentationTarget from the token currently under
// the cursor.
Optional<InstrumentationTarget> target_from_cursor(
    CXCursor cursor,
    LtlEvent &event_config) {
  CXCursorKind cursor_kind = clang_getCursorKind(cursor);

  // Get the kind of instrumentation target
  config::InstrumentationTargetKind kind;
  std::string str = convertCXString(clang_getCursorSpelling(cursor));
  
  if (str == "genericFunction") {
    cout << "Kind of genericFunction: " << clang_getCursorKindSpelling(cursor_kind) << endl;
    cout << "genericFunction mangling: " << clang_Cursor_getMangling(cursor) << endl;
  }
  
  cout << "cursor name: " << clang_getCursorSpelling(cursor) << endl;
  if (cursor_kind == CXCursor_CXXMethod) {
    kind = InstrumentationTargetKind::Method;
  } else if (cursor_kind == CXCursor_FunctionDecl) {
    cout << "function decl: " << clang_getCursorSpelling(cursor) << endl;
    kind = InstrumentationTargetKind::Function;
  } else if (cursor_kind == CXCursor_Constructor) {
    kind = InstrumentationTargetKind::Constructor;
  } else if (cursor_kind == CXCursor_Destructor) {
    kind = InstrumentationTargetKind::Destructor;
  } else if (cursor_kind == CXCursor_FunctionTemplate) {
    cout << "FunctionTemplate: " << clang_getCursorSpelling(cursor) << endl;
  } else {
    return Optional<InstrumentationTarget>();
  }

  // Get the mangled name
  string mangled_name = convertCXString(clang_Cursor_getMangling(cursor));

  // Create the instrumentation target
  InstrumentationTarget target = {
    .kind = kind,
    .event_name = event_config.event_name,
    .mangled_function_name = std::move(mangled_name)
  };

  return std::move(target);
}

}

namespace config {

// Load all instrumentation targets that the cursor can pass over into the
// passed vector.
vector<InstrumentationTarget> load_instrumentation_targets(
    CXCursor &cursor,
    LtlConfig &config) {
  // Have to use the libclang approach instead of a lambda with captures
  // because you can't pass a lambda with captures as function pointers.
  struct VisitorData {
    vector<LtlEvent>::iterator events_iterator;
    vector<InstrumentationTarget> &targets;
  };

  vector<InstrumentationTarget> targets;

  VisitorData visitorData = {
    .events_iterator = config.events.begin(),
    .targets = targets
  };

  clang_visitChildren(
    cursor,
    [](CXCursor c, CXCursor parent, CXClientData client_data) {
      VisitorData *data = static_cast<VisitorData *>(client_data);
      auto &targets = data->targets;
      LtlEvent &event = *data->events_iterator;
      CXSourceLocation location = clang_getCursorLocation(c);

      if (clang_Location_isFromMainFile(location)) {
        Optional<InstrumentationTarget> optional_target =
          target_from_cursor(c, event);
        if (optional_target.hasValue()) {
          data->events_iterator++;

          if (event.enabled) {
            targets.push_back(std::move(*optional_target));
          }
        }
      }

      return CXChildVisit_Recurse;
    },
    &visitorData);

  return targets;
}

// 
Expected<vector<InstrumentationTarget>> hydrate(LtlConfig &config) {
  // These are the include flags needed to get libclang to see my standard
  // libraries. I have hopes for inferring these from the clang context.
  // Alternatively, I may be able to mimic the process that clang and whatnot
  // use to find these flags and then offer a way to pass arguments directly
  // to the libclang translation unit from command line arguments.
  // 
  // Actually, that last option is a very good initial solution; I should
  // implement it to get rid of this awful hack.
  const char *parse_args[12] = {
    "-isystem",
    "/nix/store/88kfazwin78wi7z7g7kxzkjaip0r45zy-clang-7.0.0/include",
    "-isystem",
    "/nix/store/4v04a9c6h8r2zyw9c7v185hlfvijp3ks-compiler-rt-7.0.0-dev/include",
    "-isystem",
    "/nix/store/nly6dg3af8ycqn1x87ns1r3mrcjpv43a-gcc-7.3.0/include/c++/7.3.0",
    "-isystem",
    "/nix/store/nly6dg3af8ycqn1x87ns1r3mrcjpv43a-gcc-7.3.0/include/c++/7.3.0/x86_64-unknown-linux-gnu",
    "-isystem",
    "/nix/store/nc8m5amn9rh1fik8pn8al53yskgn5qdw-clang-wrapper-7.0.0/resource-root/include",
    "-isystem",
    "/nix/store/f3l058q0zvnzr7nvl0jj789pyvljqadw-glibc-2.27-dev/include"
  };

  std::string config_file_body = make_parsable_file(config);

  cout << "config_file_body: \n" << config_file_body << endl;

  CXUnsavedFile unsaved_file =
    unsaved_file_from_string("configfile.cpp", config_file_body);

  CXUnsavedFile unsaved_files[1] = { unsaved_file };

  CXIndex index = clang_createIndex(0, 0);

  CXTranslationUnit unit = clang_parseTranslationUnit(
    index,
    "configfile.cpp", parse_args, 12,
    unsaved_files, 1,
    CXTranslationUnit_None);

  if (unit == nullptr)
  {
    cerr << "Unable to parse translation unit. Quitting." << endl;
    exit(-1);
  }

  CXCursor cursor = clang_getTranslationUnitCursor(unit);

  vector<InstrumentationTarget> targets =
    load_instrumentation_targets(cursor, config);

  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);

  return std::move(targets);
}

unordered_map<string, InstrumentationTarget> map_by_mangled_names(
    vector<InstrumentationTarget> &targets) {
  unordered_map<string, InstrumentationTarget> target_map;

  for (auto &target : targets) {
    target_map[target.mangled_function_name] = target;
  }

  return target_map;
}

Expected<unordered_map<string, InstrumentationTarget>> parse(string &filename) {
  ifstream input_file;
  json json_obj;

  input_file.open(filename);
  input_file >> json_obj;
  input_file.close();

  // TODO: error handling with the code below for the input_file. Need to check
  // stream interface.
  //return make_error<StringError>("File not found", errc::no_such_file_or_directory);

  LtlConfig config = build_config(json_obj);

  auto targets_or_err = hydrate(config);

  if (auto err = targets_or_err.takeError()) {
    return err;
  }

  auto targets = *targets_or_err;

  auto mapped_targets = map_by_mangled_names(targets);

  return mapped_targets;
}

}
