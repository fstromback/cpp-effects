#include "pointer.h"
#include "handler_frame.h"

namespace effects {

	Shared_Ptr_Base::Shared_Ptr_Base(Shared_Count *count) : count(count) {}

	Shared_Ptr_Base::~Shared_Ptr_Base() {}

}
