#include <iostream>

using std::cout;
using std::endl;

// The `__asm__` attributes prevent the function names from being mangled
// in the output.
void runtime_ltl_entry_fn(char *str) __asm__("runtime_ltl_entry_fn");
void runtime_ltl_exit_fn(char *str) __asm__("runtime_ltl_exit_fn");

void runtime_ltl_entry_fn(char *str) {
  cout << "ANNOTATION: Entering: " << str << endl;
}

void runtime_ltl_exit_fn(char *str) {
  cout << "ANNOTATION: Exiting: " << str << endl;
}
