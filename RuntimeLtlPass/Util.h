#ifndef RUNTIME_LTL_UTIL_H
#define RUNTIME_LTL_UTIL_H

#include <ostream>
#include <string>

#include <clang-c/Index.h>

namespace config {

std::ostream& operator<<(std::ostream& stream, const CXString& str);

}

#endif
