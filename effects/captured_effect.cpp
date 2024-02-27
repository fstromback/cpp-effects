#include "captured_effect.h"

namespace effects {

	void Resume_Params::call(const Continuation_Base &cont) {
		effect->call(this, cont);
	}

}
