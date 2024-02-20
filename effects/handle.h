#pragma once
#include <exception>
#include <optional>
#include "handler_frame.h"

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

	template <typename Result, typename Function>
	class Handler_Body_Impl : public Handler_Body {
	public:
		Handler_Body_Impl(Function f) : to_call(std::move(f)) {}

		virtual void call() override {
			try {
				result_value = to_call();
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
		std::optional<Result> result_value;
		std::exception_ptr error_value;
	};

	template <typename Function>
	class Handler_Body_Impl<void, Function> {
		Handler_Body_Impl(Function f) : to_call(std::move(f)) {}

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
		std::exception_ptr error_value;
	};


	// Handle effects with a handler.
	template <typename HandlerBody>
	auto handle(HandlerBody body) -> decltype(body()) {
		// TODO: Heap-allocate with a suitable smart pointer!
		Handler_Body_Impl<decltype(body()), HandlerBody> b(std::move(body));
		Handler_Frame::call(&b);
		return b.result();
	}

}
