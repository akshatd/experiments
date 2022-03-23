#include <iostream>
#include <experimental/reflect>

enum TestEnum {
	Zero = 0,
	One,
	Theree = 3,
	TestEnumEnd,
};

void PrintTestEnum() { std::cout << "TestEnum::Zero :" << TestEnum::Zero << '\n'; }

int main(void) {
	PrintTestEnum();
	return 0;
}