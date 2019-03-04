//#include <stdio.h>
//#include <stdbool.h>
#include <iostream>

void log_enter() {
  std::cout << "Entering" << std::endl;
}

void log_exit() {
  std::cout << "Exiting" << std::endl;
}

void annotate_this() {
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
  void method() {
    std::cout << "Hey " << val << std::endl;
  }
};

int main() {
  Test test(1);
  test.method();
  return 0;
}
