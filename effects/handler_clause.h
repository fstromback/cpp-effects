#pragma once
#include <functional>
#include "continuation.h"
#include "handle_body.h"
#include "util.h"
#include "result.h"

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

	/**
	 * Handler clause that knows only the signature, but not the type of the handler.
	 */
	template <typename T>
	class Partial_Handler_Clause;

	template <typename EffectResult, typename... Args>
	class Partial_Handler_Clause<EffectResult (Args...)> : public Handler_Clause {
	public:
		Partial_Handler_Clause(size_t effect_id) : Handler_Clause(effect_id) {}

		virtual void call(Generic_Result &result_to,
						const std::tuple<Args...> &args,
						const Captured_Continuation &cont,
						Result<EffectResult> &cont_param_to) const = 0;
	};


	/**
	 * Fully bound handler.
	 */
	template <typename Result, typename Signature>
	class Bound_Handler_Clause;

	template <typename HandlerResult, typename EffectResult, typename... Args>
	class Bound_Handler_Clause<HandlerResult, EffectResult (Args...)>
		: public Partial_Handler_Clause<EffectResult (Args...)> {
	public:
		using Continuation_Type = Continuation<HandlerResult, EffectResult>;
		using Real_Function = std::function<HandlerResult (Args..., const Continuation_Type &)>;

		// Create.
		Bound_Handler_Clause(size_t effect_id, Real_Function body)
			: Partial_Handler_Clause<EffectResult (Args...)>(effect_id), body(std::move(body)) {}

		// Body of the handler.
		Real_Function body;

		virtual void call(Generic_Result &result_to,
						const std::tuple<Args...> &args,
						const Captured_Continuation &cont,
						Result<EffectResult> &cont_param_to) const override {
			Result<HandlerResult> &out = dynamic_cast<Result<HandlerResult> &>(result_to);

			Continuation_Type c(cont, out, cont_param_to);
			out.set(Tuple_Call<HandlerResult, std::tuple<Args...>>::call(body, args, c));
		}
	};

}
