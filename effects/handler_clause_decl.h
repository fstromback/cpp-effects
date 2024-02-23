#pragma once
#include "handler_clause.h"
#include "effect.h"
#include <memory>

namespace effects {

	// Create clauses conveniently.
	template <typename Signature, typename Body>
	std::shared_ptr<Bound_Handler_Clause<Signature>> clause(const Effect<Signature> &effect, Body body) {
		return std::make_unique<Bound_Handler_Clause<Signature>>(effect.id(), body);
	}

}
