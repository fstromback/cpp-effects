#include "effect.h"
#include "handler_frame.h"
#include "debug.h"

namespace effects {

	void call_handler(size_t id, Captured_Effect *effect) {
		Handler_Frame::call_handler(id, effect);
	}

}
