#pragma once
#include <cstddef>

namespace effects {

	/**
	 * This file contains definition of effects.
	 */

	template <typename Signature>
	class Effect {
	public:
		// Get a unique ID of the effect.
		size_t id() const {
			return reinterpret_cast<size_t>(this);
		}
	};

}
