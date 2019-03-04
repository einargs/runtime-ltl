cmake_minimum_required(VERSION 3.11)
project(RuntimeLtlProject)

#set(LLVM_DIR ../llvm-build/lib/cmake/llvm/)
find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(RuntimeLtlProject)

if($ENV{USING_NIX})
  install(FILES ./build/RuntimeLtlProject/RuntimeLtlProject.so DESTINATION $ENV{out})
endif()

