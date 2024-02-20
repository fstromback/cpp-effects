#pragma once
#include <exception>
#include <optional>
#include "handler_frame.h"
#include "handler.h"

namespace effects {

	/**
	 * This file implements handling of effects.
	 */


	/**
	 * Representation of the body in a format agnostic to the resulting type.
	 */
	class Handler_Body {
	public:
		// Call the handler body.
		virtual void call() = 0;
	};

	template <typename Result, typename Function, typename ReturnHandler>
	class Handler_Body_Impl : public Handler_Body {
	public:
		Handler_Body_Impl(Function f, ReturnHandler return_handler)
			: to_call(std::move(f)), return_handler(std::move(return_handler)) {}

		virtual void call() override {
			try {
				result_value = return_handler(to_call());
			} catch (...) {
				error_value = std::current_exception();
			}
		}

		Result result() const {
			if (error_value) {
				std::rethrow_exception(error_value);
			} else {
				return result_value.value();
			}
		}

	private:
		Function to_call;
		ReturnHandler return_handler;
		std::optional<Result> result_value;
		std::exception_ptr error_value;
	};

	template <typename Function, typename ReturnHandler>
	class Handler_Body_Impl<void, Function, ReturnHandler> {
		Handler_Body_Impl(Function f, ReturnHandler return_handler)
			: to_call(std::move(f)), return_handler(std::move(return_handler)) {}

		virtual void call() override {
			try {
				to_call();
			} catch (...) {
				error_value = std::current_exception();
			}
		}

		void result() const {
			if (error_value) {
				std::rethrow_exception(error_value);
			}
		}

	private:
		Function to_call;
		ReturnHandler return_handler;
		std::exception_ptr error_value;
	};


	// Handle effects with a handler.
	template <typename FromType, typename ToType, typename HandlerBody>
	ToType handle(const Handler<ToType, FromType> &handler, HandlerBody body) {
		// TODO: Heap-allocate with a suitable smart pointer!
		Handler_Body_Impl<ToType, HandlerBody, decltype(handler.return_handler)> b(std::move(body), handler.return_handler);
		Handler_Frame::call(&b, handler.clauses);
		return b.result();
	}

}
