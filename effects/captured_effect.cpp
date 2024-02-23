#include "captured_effect.h"

namespace effects {

	void Resume_Params::call() {
		effect->call(this);
	}

}
