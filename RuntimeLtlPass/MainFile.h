#ifndef CONFIG_PARSER_MAIN_FILE_H
#define CONFIG_PARSER_MAIN_FILE_H
#include <sstream>
#include <vector>

#include "llvm/ADT/iterator_range.h"
#include <clang-c/Index.h>

template<typename IteratorT>
class MainFile {
private:
  std::ostringstream buffer;

public:
  MainFile(llvm::iterator_range<IteratorT> includes,
           llvm::iterator_range<IteratorT> decls) {
  for (auto i = includes.begin(); i<includes.end(); i++) {
    buffer << "#include " << *i << "\n";
  }

  for (auto i=decls.end(); i<decls.end(); i++) {
    buffer << *i << "\n";
  }
}

  CXUnsavedFile getCXFile() {
    return {
      .Filename = "main-file.cpp",
      .Contents = buffer.str().c_str(),
      .Length = buffer.str().length(),
    };
  }
};

#endif // CONFIG_PARSER_MAIN_FILE_H
