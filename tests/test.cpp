//#include <stdio.h>
//#include <stdbool.h>
#include <iostream>

#define LTL_ENTRY_FN __attribute__((annotate("ltl_entry_fn")))
#define LTL_EXIT_FN __attribute__((annotate("ltl_exit_fn")))
#define LTL_VERIFY __attribute__((annotate("ltl_verify")))

LTL_ENTRY_FN void log_enter() {
  std::cout << "Entering" << std::endl;
}

LTL_EXIT_FN void log_exit() {
  std::cout << "Exiting" << std::endl;
}

LTL_VERIFY void annotate_this() {
  if (true) {
    std::cout << "Hello world!" << std::endl;
  } else {
    std::cout << "No" << std::endl;
  }
}

class Test {
private:
  int val;
public:
  Test(int val): val(val) {}

  LTL_VERIFY void method() {
    std::cout << "Hey " << val << std::endl;
  }
};

int main() {
  annotate_this();

  Test test(1);
  test.method();
  return 0;
}
