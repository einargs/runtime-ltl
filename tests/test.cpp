#include <iostream>

#define LTL_VERIFY __attribute__((annotate("ltl_verify")))

using std::cout;
using std::endl;

LTL_VERIFY void annotate_this() {
  if (true) {
    cout << "True path." << endl;
  } else {
    cout << "False path." << endl;
  }
}

void config_file_test() {
  cout << "Hello, I am the test function.\n";
}

class Test {
private:
  int val;
public:
  Test(int val): val(val) {
    cout << "A test object with value " << val << " has been created." << endl;
  }

  LTL_VERIFY void method() {
    cout << "Method: value is " << val << endl;
    annotate_this();
  }

  int method(int n, std::string name) {
    int incedVal = n+val;
    cout << "Incremented value: " << incedVal << " with name: " << name << endl;
    return incedVal;
  }
};

int main() {
  annotate_this();

  Test test(1);
  test.method();
  cout << "test.method(2) = " << test.method(2, "lel") << endl;

  config_file_test();
  return 0;
}
