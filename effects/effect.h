#pragma once
#include <cstddef>

namespace effects {

	class Handler_Clause;

	/**
	 * This file contains definition of effects.
	 */

	// Helper to find a handler.
	Handler_Clause *find_handler(size_t id);


	template <typename Signature>
	class Effect;

	template <typename Result, typename... Args>
	class Effect<Result (Args...)> {
	public:
		// Get a unique ID of the effect.
		size_t id() const {
			return reinterpret_cast<size_t>(this);
		}

		// Call the effect.
		Result operator ()(Args&& ...args) {
			Handler_Clause *found = find_handler(id());
			// TODO: If found, save everyhting and resume on the original stack.

			return Result{};
		}
	};

}
