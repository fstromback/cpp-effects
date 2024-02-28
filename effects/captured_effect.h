#pragma once
#include <tuple>
#include <optional>
#include "handle_body.h"
#include "handler_clause.h"
#include "util.h"

namespace effects {

	class Captured_Effect;

	/**
	 * Parameters needed to call a handler from a Captured_Effect.
	 */
	struct Resume_Params {
		// Result should be stored here.
		Handle_Body *result_to = nullptr;

		// Handler clause to call.
		const Handler_Clause *to_call = nullptr;

		// Captured continuation.
		const Captured_Continuation *continuation = nullptr;
	};


	/**
	 * This file contains a data type that contains the parameters passed to an effect, in
	 * preparation for calling a corresponding handler.
	 */
	class Captured_Effect {
	public:
		// Destructor.
		virtual ~Captured_Effect() = default;

		// Call the captured effect.
		virtual void call(Resume_Params params) = 0;
	};


	/**
	 * Specialization.
	 */
	template <typename Result, typename... Args>
	class Bound_Captured_Effect : public Captured_Effect {
	public:
		// Create.
		Bound_Captured_Effect(Args&& ...args) : args(std::forward<Args...>(args)...) {}

		// The result produced by the effect.
		effects::Result<Result> result;

		// Call.
		virtual void call(Resume_Params params) override {
			using Handler_Type = Partial_Handler_Clause<Result (Args...)>;

			// Note: We can use static_cast, but we use dynamic_cast to get a "free" assert of the types.
			const Handler_Type &handler = dynamic_cast<const Handler_Type &>(*params.to_call);

			handler.call(params.result_to->generic_result(), args, *params.continuation, result);
		}

	private:
		// Parameters.
		std::tuple<Args...> args;
	};

}
