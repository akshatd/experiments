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

int main(void) {
	Destroyer d;
	d.~Destroyer();
	return 0;
}
