{ pkgs ? import <nixpkgs> {} }:

let
  llvmPkgs = pkgs.llvmPackages_7;
  clang = llvmPkgs.clang-unwrapped;
in import ./runtime-ltl.nix {
  inherit (llvmPkgs) clang-unwrapped;
  llvm = pkgs.llvm_7;
  inherit (pkgs) stdenv cmake python3 ninja;
}
