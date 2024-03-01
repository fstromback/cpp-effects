#pragma once
#include <atomic>
#include <cstddef>
#include "debug.h"

namespace effects {

	/**
	 * Counter for shared ptr.
	 */
	class Shared_Count {
	public:
		// Create.
		Shared_Count() = default;

		// Default dtor to allow inheritance.
		virtual ~Shared_Count() = default;

		// Reference count. Initialized to 1.
		std::atomic<size_t> refs = 1;

		// Increase the references.
		void ref() {
			++refs;
		}

		// Decrease the references. Returns "true" if we should delete everything.
		void deref() {
			if (--refs == 0) {
				delete this;
			}
		}
	};

	/**
	 * Version of the count above that also stores a pointer, so that it can be deallocated.
	 */
	template <typename T>
	class Shared_Separate_Count : public Shared_Count {
	public:
		// Create.
		Shared_Separate_Count(T *object) : object(object) {}

		// Destroy.
		~Shared_Separate_Count() {
			delete object;
		}

		// The data.
		T *object;
	};

	/**
	 * Version of the count above that also stores an instance of an object in the same allocation.
	 */
	template <typename T>
	class Shared_Inline_Count : public Shared_Count {
	public:
		// Create.
		template <typename... Args>
		Shared_Inline_Count(Args &&...args) : data(std::forward<Args>(args)...) {}

		// The data in the allocation.
		T data;
	};


	/**
	 * Base class, to allow managing the Shared_Ptr in the handler_frame:s.
	 */
	class Shared_Ptr_Base {
	public:
		// Track our life-cycle.
		Shared_Ptr_Base(Shared_Count *count);

		~Shared_Ptr_Base();

	protected:
		// The count variable. Not updated by the destructor, only accessible here.
		Shared_Count *count;

		// Allow use from the pointer set.
		friend class Pointer_Set;
	};

	/**
	 * A version of shared_ptr that accounts for the oddities of continuations when doing its
	 * reference counting.
	 */
	template <typename T>
	class Shared_Ptr : public Shared_Ptr_Base {
	public:
		// Create an empty pointer.
		Shared_Ptr() : Shared_Ptr_Base(nullptr), object(nullptr) {}

		// Create from an existing allocation.
		explicit Shared_Ptr(T *object) : Shared_Ptr_Base(new Shared_Separate_Count(object)), object(object) {}

		// Copy.
		Shared_Ptr(const Shared_Ptr<T> &src) : Shared_Ptr_Base(src.count), object(src.object) {
			if (count)
				count->ref();
		}

		Shared_Ptr &operator =(const Shared_Ptr<T> &src) {
			if (src.count)
				src.count->ref();
			if (count)
				count->deref();

			object = src.object;
			count = src.count;

			return *this;
		}

		// Move.
		Shared_Ptr(Shared_Ptr<T> &&src) : Shared_Ptr_Base(src.count), object(src.object) {
			src.object = nullptr;
			src.count = nullptr;
		}

		Shared_Ptr &&operator =(Shared_Ptr<T> &&src) {
			// Note: We could simply swap the two here. This makes it easier to catch bugs, however.
			if (&src == this)
				return *this;

			if (count)
				count->deref();

			object = src.object;
			count = src.count;

			src.object = nullptr;
			src.count = nullptr;

			return *this;
		}

		// Destroy.
		~Shared_Ptr() {
			if (count)
				count->deref();
		}

		// Convert from another type.
		template <typename U>
		Shared_Ptr(const std::enable_if<
				std::is_convertible_v<U *, T *> && !std::is_same_v<U *, T *>,
				Shared_Ptr<U>> &src) : Shared_Ptr_Base(src.count), object(src.object) {
			if (count)
				count->ref();
		}

		template <typename U, typename = std::enable_if<std::is_convertible_v<T *, U *> && !std::is_same_v<U *, T *>>>
		operator Shared_Ptr<U>() const {
			return Shared_Ptr<U>(count, object);
		}

		// Get the pointer.
		T *get() const {
			return object;
		}

		T &operator *() const {
			return object;
		}

		T *operator ->() const {
			return object;
		}

		// Check if null.
		operator bool() const {
			return object != nullptr;
		}

	private:
		// The pointer itself.
		T *object;

	private:
		// Helper.
		Shared_Ptr(Shared_Count *c, T *o) : Shared_Ptr_Base(c), object(o) {
			if (c)
				c->ref();
		}

		// Create from an inline allocation.
		Shared_Ptr(Shared_Inline_Count<T> *count)
			: Shared_Ptr_Base(count), object(&count->data) {}

		template <typename U, typename... Args>
		friend Shared_Ptr<U> make_shared(Args && ...args);

		template <typename U>
		friend class Shared_Ptr;
	};

	// Check for equality.
	template <typename T, typename U>
	bool operator ==(const Shared_Ptr<T> &a, const Shared_Ptr<U> &b) {
		return a.get() == b.get();
	}

	template <typename T, typename U>
	bool operator !=(const Shared_Ptr<T> &a, const Shared_Ptr<U> &b) {
		return a.get() != b.get();
	}

	// Alias that looks more like std::shared_ptr.
	template <typename T>
	using shared_ptr = Shared_Ptr<T>;

	// Create a shared pointer.
	template <typename T, typename... Args>
	Shared_Ptr<T> make_shared(Args && ...args) {
		return Shared_Ptr<T>(new Shared_Inline_Count<T>(std::forward<Args>(args)...));
	}

}
