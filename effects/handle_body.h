#pragma once
#include <exception>
#include <optional>

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
	};

	// A handler body with a result.
	template <typename Result>
	class Handle_Body_Result : public Handle_Body {
	public:
		Result result() const {
			if (error_value) {
				std::rethrow_exception(error_value);
			} else {
				return result_value.value();
			}
		}

		void set_result(const Result &v) {
			result_value = v;
		}

		void set_error(std::exception_ptr ptr) {
			error_value = std::move(ptr);
		}

	private:
		std::optional<Result> result_value;
		std::exception_ptr error_value;
	};

	template <>
	class Handle_Body_Result<void> : public Handle_Body {
	public:
		void result() const {
			if (error_value) {
				std::rethrow_exception(error_value);
			}
		}

		void set_error(std::exception_ptr ptr) {
			error_value = std::move(ptr);
		}

	private:
		std::exception_ptr error_value;
	};

	template <typename Result, typename Function, typename ReturnHandler>
	class Handle_Body_Impl : public Handle_Body_Result<Result> {
	public:
		Handle_Body_Impl(Function f, ReturnHandler return_handler)
			: to_call(std::move(f)), return_handler(std::move(return_handler)) {}

		virtual void call() override {
			try {
				this->set_result(return_handler(to_call()));
			} catch (...) {
				this->set_error(std::current_exception());
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
				this->set_error(std::current_exception());
			}
		}

	private:
		Function to_call;
		ReturnHandler return_handler;
	};

}
