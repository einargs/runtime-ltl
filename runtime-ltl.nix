{ clang-unwrapped, llvm, cmake, python3, ninja
, stdenv
}:

let 
in stdenv.mkDerivation {
  name = "runtime-ltl";

  src = ./.;

  BUILDING_WITH_NIX = true;

  nativeBuildInputs = [
    cmake python3 ninja 
  ];

  buildInputs = [
    llvm
    clang-unwrapped
  ];
}
