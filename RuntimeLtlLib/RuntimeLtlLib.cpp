#include <iostream>

using std::cout;
using std::endl;

// The `__asm__` attributes prevent the function names from being mangled
// in the output.
void runtime_ltl_entry_fn() __asm__("runtime_ltl_entry_fn");
void runtime_ltl_exit_fn() __asm__("runtime_ltl_exit_fn");

void runtime_ltl_entry_fn() {
  cout << "Entering" << endl;
}

void runtime_ltl_exit_fn() {
  cout << "Exiting" << endl;
}
