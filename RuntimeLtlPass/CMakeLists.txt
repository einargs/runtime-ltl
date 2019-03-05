#add_library(RuntimeLtlProject MODULE RuntimeLtlProject.cpp)
#target_compile_features(RuntimeLtlProject PRIVATE cxx_range_for cxx_auto_type)

add_llvm_library( RuntimeLtlPass MODULE BUILDTREE_ONLY
  RuntimeLtlPass.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
