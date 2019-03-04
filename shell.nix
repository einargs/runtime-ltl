{ pkgs ? import <nixpkgs> {} }:

with pkgs;

let 
  clang = llvmPackages_7.clang-unwrapped;
  llvm = llvm_7;
in mkShell {
  buildInputs = [
    cmake ninja clang llvm
  ];
  
  USING_NIX_SHELL = true;
  clang7 = "${clang_7}/bin/clang";
}
