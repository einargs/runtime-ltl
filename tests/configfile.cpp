#include "./no-inline.cpp"

void config_file_test();

void Test::method(int, std::string);

void annotate_this();

Test::Test(int);

// Note that the generics need to be specialized for a given type.
void GenericClass<int>::genericMethod(int);

