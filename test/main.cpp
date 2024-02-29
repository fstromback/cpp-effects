#include <iostream>
#include <string>
#include "effects/effects.h"

using namespace effects;

Effect<int (int)> my_effect;

int cont(int x) { return x; }

Handler<int, int> my_handler{
	{
		{
			my_effect,
			[](int param, const Continuation<int, int> &cont) {
				std::cout << "In handler: " << param << std::endl;
				int r = cont(param + 1);
				std::cout << "Got " << r << " from continuation." << std::endl;
				int s = cont(param + 2);
				std::cout << "Got " << s << " from continuation." << std::endl;
				return r + s;
			}
		}
	},
	[](int result) {
		std::cout << "Return handler: " << result << std::endl;
		return result + 1;
	}
};

class Heap {
public:
	Heap() {
		std::cout << "Created heap class!" << std::endl;
	}

	~Heap() {
		std::cout << "Destroyed heap class!" << std::endl;
	}
};

int main() {
	int result = handle(my_handler, [](){
		std::cout << "Hello!" << std::endl;
		Shared_Ptr<Heap> h = make_shared<Heap>();
		int from_effect = my_effect(1);
		std::cout << "From the effect: " << from_effect << std::endl;
		return from_effect * 2;
	});

	std::cout << "Result: " << result << std::endl;

	return 0;
}
