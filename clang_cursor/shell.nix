{ pkgs ? import <nixpkgs> {} }:

with pkgs;

let
  libclang = llvmPackages_7.libclang;
in mkShell {
  buildInputs = [
    libclang clang_7
  ];

  inherit libclang;
  libclang_out = libclang.out;
  libclang_lib = libclang.lib;
}
