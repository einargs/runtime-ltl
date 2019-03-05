cmake_minimum_required(VERSION 3.11)
project(RuntimeLtlProject)

#set(LLVM_DIR $LLVM_BUILD_ROOT/lib/cmake/llvm/)
find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(RuntimeLtlPass)
add_subdirectory(RuntimeLtlLib)

if($ENV{BUILDING_WITH_NIX})
  install(FILES ./build/RuntimeLtlPass/RuntimeLtlPass.so DESTINATION $ENV{out})
  install(FILES ./build/RuntimeLtlLib/RuntimeLtlLib.so DESTINATION $ENV{out})
endif()

