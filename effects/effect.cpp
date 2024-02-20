#include "effect.h"
#include "handler_frame.h"

namespace effects {

	Handler_Clause *find_handler(size_t id) {
		return Handler_Frame::find(id);
	}

}
