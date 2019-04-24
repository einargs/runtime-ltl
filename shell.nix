{ pkgs ? import <nixpkgs> {} }:

with pkgs;

let 
  clang = llvmPackages_7.clang-unwrapped;
  libclang = llvmPackages_7.libclang;
  llvm = llvm_7;
  spot = import ./spot.nix {
    inherit stdenv fetchTarball python37;
  };
in mkShell {
  buildInputs = [
    cmake ninja clang libclang llvm spot
  ];
  
  USING_NIX_SHELL = true;
  clang7 = "${clang_7}/bin/clang";
  clangxx7 = "${clang_7}/bin/clang++";

  inherit libclang;
  libclang_out = libclang.out;
  libclang_lib = libclang.lib;
}
