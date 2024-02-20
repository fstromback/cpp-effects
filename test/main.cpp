#include <iostream>
#include <string>
#include "effects/effects.h"

using namespace effects;

Effect<int (int)> my_effect;

Handler<int, int> my_handler{
	{
		clause(my_effect, [](int param) {
			std::cout << "In handler: " << param << std::endl;
			return param + 1;
		})
	},
	[](int result) {
		std::cout << "Return handler: " << result << std::endl;
		return result + 1;
	}
};

int main() {
	int result = handle(my_handler, [](){
		std::cout << "Hello!" << std::endl;
		int from_effect = my_effect(1);
		std::cout << "From the effect: " << from_effect << std::endl;
		return from_effect * 2;
	});

	std::cout << "Result: " << result << std::endl;

	return 0;
}
