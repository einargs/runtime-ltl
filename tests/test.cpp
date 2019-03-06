#include <iostream>

#define LTL_VERIFY __attribute__((annotate("ltl_verify")))

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
    annotate_this();
  }
};

int main() {
  annotate_this();

  Test test(1);
  test.method();
  return 0;
}
