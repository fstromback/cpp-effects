#include "continuation.h"
#include "handler_frame.h"

namespace effects {

	void Captured_Continuation::resume() const {
		Handler_Frame::resume_continuation(*this);
	}

}
