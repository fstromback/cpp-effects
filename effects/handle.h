#pragma once
#include "handler_frame.h"
#include "handler.h"
#include "handle_body.h"

namespace effects {

	/**
	 * This file implements handling of effects.
	 */


	// Handle effects with a handler.
	template <typename FromType, typename ToType, typename HandleBody>
	ToType handle(const Handler<ToType, FromType> &handler, HandleBody body) {
		// TODO: Heap-allocate with a suitable smart pointer!
		Handle_Body_Impl<ToType, HandleBody, decltype(handler.return_handler)> b(std::move(body), handler.return_handler);
		Handler_Frame::call(&b, handler.clauses);
		return b.result();
	}

}
