#pragma once
#include "effect.h"
#include <functional>

namespace effects {

	/**
	 * Implementation of individual handler clauses.
	 */

	/**
	 * Generic handler clause.
	 */
	class Handler_Clause {
	public:
		// Create.
		Handler_Clause(size_t effect_id) : id(effect_id) {}

		// Unique ID for the handled effect.
		const size_t id;
	};

	template <typename Signature>
	class Bound_Handler_Clause : public Handler_Clause {
	public:
		// Create.
		Bound_Handler_Clause(const Effect<Signature> &effect, std::function<Signature> body)
			: Handler_Clause(effect.id()), effect(effect), body(std::move(body)) {}

		// The effect being handled.
		const Effect<Signature> &effect;

		// Body of the handler.
		std::function<Signature> body;
	};

	// Create clauses conveniently.
	template <typename Signature, typename Body>
	Bound_Handler_Clause<Signature> clause(const Effect<Signature> &effect, Body body) {
		return Bound_Handler_Clause<Signature>(effect, body);
	}

}
