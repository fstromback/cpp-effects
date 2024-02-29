#pragma once
#include "stack.h"
#include "result.h"
#include "debug.h"
#include <vector>

namespace effects {

	/**
	 * Captured contents of a continuation.
	 *
	 * This data can be used to create a proper Continuation<> below.
	 */
	class Captured_Continuation {
	public:
		// Create a copy of a stack.
		Captured_Continuation(size_t depth) {
			frames.reserve(depth);
		}

		// Store stack frames.
		std::vector<Stack_Mirror> frames;

		// Resume a captured continuation.
		void resume() const;
	};

	/**
	 * A continuation with parameters that can be invoked.
	 *
	 * We currently do not allow copying the continuation, as that would allow cycles in refcounts
	 * easily.
	 */
	template <typename Result, typename Param>
	class Continuation {
	public:
		// Disable copying the continuation to make resource management easier.
		Continuation(const Continuation &) = delete;
		Continuation &operator =(const Continuation &) = delete;

		// Create a continuation from a captured continuation, as well as where to retrieve the result from.
		Continuation(Captured_Continuation src, effects::Result<Result> &result, effects::Result<Param> &param)
			: src(src), result(result), param(param) {}

		// Destroy the continuation.
		~Continuation() {
			PLN("TODO: Should destroy any pointers that are in the captured continuation here!");
		}

		// Call the continuation.
		Result operator() (Param param) const {
			// Restore all stacks first. The parameter is stored on the stack of the receiving
			// piece, so if we store it before restoring stacks, the value will be overwritten.
			for (const Stack_Mirror &s : src.frames)
				s.restore();

			// Now, we can set the result...
			this->param.set(std::move(param));

			// ...and resume the old stack.
			src.resume();

			// When we are back here, the continuation has finished executing.
			return this->result.result();
		}

	private:
		// Captured stack frames.
		Captured_Continuation src;

		// Where is the result from executing the continuation stored?
		effects::Result<Result> &result;

		// Where should we store the parameter?
		effects::Result<Param> &param;
	};

}
