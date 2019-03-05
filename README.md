# Runtime LTL
This is an undergraduate research project aiming towards creating a runtime
verification process for C++ code that will check the function call graph
against an LTL formula.

## Process
This is done by creating an LLVM IR pass that inserts calls to a verification
system at the beginning and end of annotated functions.

Functions are annotated like so:
```c
__attribute__((annotate("ltl_verify"))) void annotate_this() {
  printf("Hello world!\n");
}
```

## Requirements
- CMake >= 3.11
- ninja 1.8.2
- LLVM 7 (`llvm-config` and `lli` should be in your `PATH`)
- Clang 7

## Building
To build, please run `configure`, which will setup a `build` directory to use
CMake with Ninja as the target build system. If you are familiar with CMake,
feel free to set this up to use other build systems. The configure script will
perform an initial build of the shared object; afterwards, to re-build upon
changes to the source, run `cmake --build $PROJECT_ROOT/build`.

## Using Nix
If you use [Nix](https://nixos.org/nix/) you can simply run `shell.nix` to
install the required dependencies. Further, you can build the shared object
with `nix-build`. Make sure that the `build` directory does not exist, as it
will be copied by `nix-build` and conflict with the new `build` directory the
nix build process will try to generate.

During development it is recommended to use the process described in
[Building](#building) to avoid rebuilding the shared object from scratch after
every change.

## Using `run-pass.sh`
`run-pass.sh` is a script for quickly trying out the runtime ltl plugin on
an example `c` file. It is designed to deal with both `nix` and non-`nix`
environments. The script is commented, but rough, as I am not very familiar
with bash scripting.

The script will print to STDOUT the generated IR and then execute it using the
`lli` interpreter.

Syntax:
```
[PASS_SO=<plugin shared object>] ./run-pass.sh <source input> [<IR output file>]
```

If `PASS_SO` is an environment variable, it will use that as the shared object
for dynamically linking the plugin. The first argument is the source file,
and the second argument is an optional file to output the generated IR to.
