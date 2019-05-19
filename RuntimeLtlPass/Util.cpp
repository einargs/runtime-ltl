#include "./Util.h"

#include <ostream>
#include <clang-c/Index.h>  // This is libclang.

using std::ostream;

namespace config {

ostream& operator<<(ostream& stream, const CXString& str)
{
  stream << clang_getCString(str);
  clang_disposeString(str);
  return stream;
}

std::string convertCXString(const CXString &str) {
  std::string cpp_string(clang_getCString(str));
  clang_disposeString(str);
  return cpp_string;
}

}

