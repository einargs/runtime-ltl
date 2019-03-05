#include <iostream>

using std::cout;
using std::endl;
void runtime_ltl_entry_fn() asm("runtime_ltl_entry_fn");
void runtime_ltl_exit_fn() asm("runtime_ltl_exit_fn");

void runtime_ltl_entry_fn() {
  cout << "Entering" << endl;
}

void runtime_ltl_exit_fn() {
  cout << "Exiting" << endl;
}
