#include "pointer.h"
#include "handler_frame.h"

namespace effects {

	Shared_Ptr_Base::Shared_Ptr_Base(Shared_Count *count) : count(count) {
		Handler_Frame::add_shared_ptr(this);
	}

	Shared_Ptr_Base::~Shared_Ptr_Base() {
		Handler_Frame::remove_shared_ptr(this);
	}

}
