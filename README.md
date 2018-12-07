# Runtime LTL Verificiation
This project is aimed towards creating a clang plugin that during compile-time
generates code that will, during runtime, verify that the function call order
fulfills the limits specified by a LTL (Linear Temporal Logic) expression.

## TODO
TODO:
- [ ] Investigate docker or similar container for reproducible build
environment.
- [ ] Investigate how to perform re-writes within the AST walker.
- [ ] Locate library for parsing LTL expressions.
- [ ] Design runtime verification system.
- [ ] Implement automated test system.
- [ ] Implement build system based on CMake and Ninja
(as LLVM uses these tools).
