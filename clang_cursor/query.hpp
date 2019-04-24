#include <string>
#include <tuple>

using namespace std;

namespace what {
  class MyClass {
  private:
    int val;
  public:
    MyClass(int val): val(val) {}

    int method() {
      return val;
    }
  };
};

std::tuple<int,int> fn() {

}

std::string fn_ret_str(int arg) {
  return "";
}
