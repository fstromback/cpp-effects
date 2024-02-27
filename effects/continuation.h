#pragma once

namespace effects {

	/**
	 * A continuation that can be invoked.
	 *
	 * Contains a list of stacks that have been saved and that should be resumed.
	 */
	class Continuation_Base {
	public:
		virtual ~Continuation_Base() = default;
	};


	/**
	 * Parameterized continuation.
	 */
	template <typename Result, typename Param>
	class Continuation {
	public:
		Continuation(const Continuation &) = delete;
		Continuation &operator =(const Continuation &) = delete;

		// Call the continuation.
		Result operator() (Param param) const {
			(void)param;
			// ...
			return Result{};
		}
	};

}
