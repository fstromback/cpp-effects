#pragma once
#include <cstddef>
#include <exception>
#include "captured_effect.h"
#include "continuation.h"

namespace effects {

	/**
	 * This file contains definition of effects.
	 */

	// Helper to call a handler.
	void call_handler(size_t id, Captured_Effect *captured);


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
			Bound_Captured_Effect<Result, Args...> bound(
				std::forward<Args...>(args)...);

			call_handler(id(), &bound);

			return bound.result.result();
		}
	};

	// Thrown if a handler is not present.
	class no_handler : std::exception {
	public:
		no_handler() = default;
		virtual const char *what() const noexcept {
			return "No handler was found for an effect.";
		}
	};

}
