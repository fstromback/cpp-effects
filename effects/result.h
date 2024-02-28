#pragma once
#include <exception>
#include <optional>

namespace effects {

	/**
	 * Generic result to avoid dynamic casts.
	 */
	class Generic_Result {
	public:
		virtual ~Generic_Result() = default;
	};

	/**
	 * Generic implementation of a result that can be posted between different threads.
	 */
	template <class T>
	class Result : public Generic_Result {
	public:
		// Create, initially empty.
		Result() {}

		// Get the result.
		const T &result() const {
			if (error) {
				std::rethrow_exception(error);
			} else {
				return value.value();
			}
		}

		// Set the result.
		void set(T value) {
			value = std::move(value);
		}

		// Set an error. Called from a try-block.
		void set_error(std::exception_ptr ptr) {
			error = std::move(ptr);
		}

	private:
		// The result.
		std::optional<T> value;

		// Exception, if any.
		std::exception_ptr error;
	};


	/**
	 * Specialization for void.
	 */
	template <>
	class Result<void> : public Generic_Result {
	public:
		// Create, initially empty.
		Result() {}

		// Get the result.
		void result() const {
			if (error) {
				std::rethrow_exception(error);
			}
		}

		// Set the result.
		void set() {}

		// Set an error. Called from a try-block.
		void set_error(std::exception_ptr ptr) {
			error = std::move(ptr);
		}

	private:
		// Exception, if any.
		std::exception_ptr error;
	};

}
