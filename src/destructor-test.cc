#include <iostream>

class Destroyer {
 private:
	/* data */

 public:
	Destroyer(/* args */);
	~Destroyer();
};

Destroyer::Destroyer(/* args */) {}

Destroyer::~Destroyer() { std::cout << "destroying~\n"; }

int main(int argc, char const *argv[]) {
	Destroyer d;
	d.~Destroyer();
	return 0;
}
