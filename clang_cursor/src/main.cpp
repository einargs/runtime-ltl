#include <iostream>
#include <sstream>

#include <clang-c/Index.h>  // This is libclang.

using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;

ostream& operator<<(ostream& stream, const CXString& str)
{
  stream << clang_getCString(str);
  clang_disposeString(str);
  return stream;
}

CXUnsavedFile buildMainFile(vector<string> includes, vector<string> statements) {
  ostringstream output;
  for (auto i=includes.cbegin(); i < includes.cend(); i++) {
    
  }
}

int main()
{
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
    "query.hpp", parse_args, 12,
    nullptr, 0,
    CXTranslationUnit_None);
  if (unit == nullptr)
  {
    cerr << "Unable to parse translation unit. Quitting." << endl;
    exit(-1);
  }

  CXCursor cursor = clang_getTranslationUnitCursor(unit);
  clang_visitChildren(
    cursor,
    [](CXCursor c, CXCursor parent, CXClientData client_data)
    {
      CXSourceLocation location = clang_getCursorLocation(c);
      if (clang_Location_isFromMainFile(location)) {
        CXModule module = clang_Cursor_getModule(c);
        CXCursorKind kind = clang_getCursorKind(c);
        if (kind == CXCursor_FunctionDecl || kind == CXCursor_CXXMethod) {
          CXType type = clang_getCursorType(c);
          CXType resultType = clang_getResultType(type);

          cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
            << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "'\n"
            << "Type: " << clang_getTypeSpelling(type) << "\n"
            << "TypeKind: " << clang_getTypeKindSpelling(type.kind) << "\n"
            << "ResultType: " << clang_getTypeSpelling(resultType) << "\n"
            << "ResultTypeKind: " << clang_getTypeKindSpelling(resultType.kind) << "\n";

          cout << "Mangling: " << clang_Cursor_getMangling(c) << "\n\n";
        }
      }
      return CXChildVisit_Recurse;
    },
    nullptr);

  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);
}
