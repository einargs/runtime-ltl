#add_library(RuntimeLtlProject MODULE RuntimeLtlProject.cpp)
#target_compile_features(RuntimeLtlProject PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
#set_target_properties(RuntimeLtlProject PROPERTIES COMPILE_FLAGS "-fno-rtti")
# If we don't need RTTI or EH, there's no reason to export anything
# from the hello plugin.
#if( NOT LLVM_REQUIRES_RTTI )
#  if( NOT LLVM_REQUIRES_EH )
#    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/RuntimeLtlProject.exports)
#  endif()
#endif()

#if(WIN32 OR CYGWIN)
#  set(LLVM_LINK_COMPONENTS Core Support)
#endif()

add_llvm_library( RuntimeLtlProject MODULE BUILDTREE_ONLY
  RuntimeLtlProject.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
