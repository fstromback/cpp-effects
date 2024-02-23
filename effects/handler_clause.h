#pragma once
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

		// Destructor.
		virtual ~Handler_Clause() = default;

		// Unique ID for the handled effect.
		const size_t id;
	};

	template <typename Signature>
	class Bound_Handler_Clause : public Handler_Clause {
	public:
		// Create.
		Bound_Handler_Clause(size_t effect_id, std::function<Signature> body)
			: Handler_Clause(effect_id), body(std::move(body)) {}

		// Body of the handler.
		std::function<Signature> body;
	};

}
