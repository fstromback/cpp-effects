#pragma once
#include <functional>
#include "continuation.h"
#include "handle_body.h"
#include "util.h"

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

		virtual void call(Handle_Body &result_to, const std::tuple<Args...> &args, const Continuation_Base &cont) const = 0;
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

		virtual void call(Handle_Body &result_to, const std::tuple<Args...> &args, const Continuation_Base &cont) const {
			using Body_Type = Handle_Body_Result<HandlerResult>;

			Body_Type &out = dynamic_cast<Body_Type &>(result_to);
			const Continuation_Type &c = dynamic_cast<const Continuation_Type &>(cont);

			out.set_result(Tuple_Call<HandlerResult, std::tuple<Args...>>::call(body, args, c));
		}
	};

}
