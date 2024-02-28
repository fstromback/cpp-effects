#pragma once
#include <exception>
#include <optional>
#include "result.h"

namespace effects {

	/**
	 * Representation of the body in a format agnostic to the resulting type.
	 */
	class Handle_Body {
	public:
		// Destructor.
		virtual ~Handle_Body() = default;

		// Call the handler body.
		virtual void call() = 0;

		// Get the generic result.
		virtual Generic_Result &generic_result() = 0;
	};

	// A handler body with a result.
	template <typename Result>
	class Handle_Body_Result : public Handle_Body {
	public:
		effects::Result<Result> result;

		// Get the generic result.
		virtual Generic_Result &generic_result() override {
			return result;
		}
	};

	template <typename Result, typename Function, typename ReturnHandler>
	class Handle_Body_Impl : public Handle_Body_Result<Result> {
	public:
		Handle_Body_Impl(Function f, ReturnHandler return_handler)
			: to_call(std::move(f)), return_handler(std::move(return_handler)) {}

		virtual void call() override {
			try {
				this->result.set(return_handler(to_call()));
			} catch (...) {
				this->result.set_error(std::current_exception());
			}
		}

	private:
		Function to_call;
		ReturnHandler return_handler;
	};

	template <typename Function, typename ReturnHandler>
	class Handle_Body_Impl<void, Function, ReturnHandler> : public Handle_Body_Result<void> {
		Handle_Body_Impl(Function f, ReturnHandler return_handler)
			: to_call(std::move(f)), return_handler(std::move(return_handler)) {}

		virtual void call() override {
			try {
				to_call();
			} catch (...) {
				this->result.set_error(std::current_exception());
			}
		}

	private:
		Function to_call;
		ReturnHandler return_handler;
	};

}
