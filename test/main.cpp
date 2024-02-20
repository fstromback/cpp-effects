#include <iostream>
#include "effects/effects.h"

int main() {
	int result = effects::handle([](){
		std::cout << "Hello!" << std::endl;
		return 1;
	});

	std::cout << "Result: " << result << std::endl;

	return 0;
}
