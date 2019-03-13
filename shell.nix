{ pkgs ? import <nixpkgs> {} }:

with pkgs;

let 
  clang = llvmPackages_7.clang-unwrapped;
  llvm = llvm_7;
  spot = import ./spot.nix {
    inherit stdenv fetchTarball python37;
  };
in mkShell {
  buildInputs = [
    cmake ninja clang llvm spot
  ];
  
  USING_NIX_SHELL = true;
  clang7 = "${clang_7}/bin/clang";
  clangxx7 = "${clang_7}/bin/clang++";
}
