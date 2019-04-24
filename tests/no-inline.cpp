#include <string>
#include <iostream>

using std::cout;
using std::endl;

template<typename T>
class GenericClass {
private:
  T value;

public:
  GenericClass(T t): value(t) {}

  void genericMethod(T arg) {
    cout << "genericMethod called " << value << ' ' << arg << endl;
  }
};

template<typename T>
void genericFunction(T t) {
  cout << "genericFunction called " << t << endl;
}

void annotate_this() {
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

  void method() {
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

  GenericClass<int> genericObj(123);
  genericObj.genericMethod(456);

  genericFunction<std::string>("Called generically");
  genericFunction(89);

  return 0;
}
