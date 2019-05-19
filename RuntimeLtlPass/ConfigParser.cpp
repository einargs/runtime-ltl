#include "./ConfigParser.h"

#include <iostream>
#include <sstream>

#include <clang-c/Index.h>  // This is libclang.

#include "./Util.h"

using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;
using std::unique_ptr;
using std::cout;
using std::cerr;
using std::endl;

using llvm::Expected;
using llvm::make_unique;
using llvm::Optional;

namespace config {

Optional<InstrumentationTarget> target_from_cursor(CXCursor cursor) {
  CXCursorKind cursor_kind = clang_getCursorKind(cursor);

  // Get the kind of instrumentation target
  InstrumentationTargetKind kind;
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
    .mangled_name = std::move(mangled_name)
  };

  return std::move(target);
}

void load_instrumentation_targets(
    CXCursor &cursor,
    unique_ptr<vector<InstrumentationTarget>> &targets_ptr) {
  struct VisitorData {
    unique_ptr<vector<InstrumentationTarget>> &targets;
  };

  VisitorData data = { .targets = targets_ptr };

  clang_visitChildren(
    cursor,
    [](CXCursor c, CXCursor parent, CXClientData client_data) {
      auto &targets = static_cast<VisitorData *>(client_data)->targets;
      CXSourceLocation location = clang_getCursorLocation(c);
      if (clang_Location_isFromMainFile(location)) {
        Optional<InstrumentationTarget> optional_target = target_from_cursor(c);
        if (optional_target.hasValue()) {
          targets->push_back(std::move(*optional_target));
        }
      }
      return CXChildVisit_Recurse;
    },
    &data);
}

Expected<unique_ptr<vector<InstrumentationTarget>>> parse(std::string config_file) {
  unique_ptr<vector<InstrumentationTarget>> targets_ptr =
    make_unique<vector<InstrumentationTarget>>();

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
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit unit = clang_parseTranslationUnit(
    index,
    config_file.c_str(), parse_args, 12,
    nullptr, 0,
    CXTranslationUnit_None);
  if (unit == nullptr)
  {
    cerr << "Unable to parse translation unit. Quitting." << endl;
    exit(-1);
  }

  CXCursor cursor = clang_getTranslationUnitCursor(unit);

  load_instrumentation_targets(cursor, targets_ptr);

  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);

  return std::move(targets_ptr);
}

};
