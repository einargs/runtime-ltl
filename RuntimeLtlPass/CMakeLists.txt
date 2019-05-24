#add_library(RuntimeLtlProject MODULE RuntimeLtlProject.cpp)
#target_compile_features(RuntimeLtlProject PRIVATE cxx_range_for cxx_auto_type)

add_llvm_library( RuntimeLtlPass MODULE BUILDTREE_ONLY
  RuntimeLtlPass.cpp
  ConfigParser.cpp
  Util.cpp
  Ast.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )

target_link_libraries(RuntimeLtlPass libclang)
