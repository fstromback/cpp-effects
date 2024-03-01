#pragma once
#include "pointer.h"
#include <vector>

namespace effects {

	/**
	 * Stores a set of Shared_Ptrs in order to keep track of them during continuations.
	 *
	 * Used to save and restore them during the execution of continuations.
	 */
	class Pointer_Set {
	public:
		// Create an empty set.
		Pointer_Set();

		// Create, initialize from a container of Shared_Ptr_Base *
		template <typename Container>
		explicit Pointer_Set(const Container &c) {
			elements.reserve(c.size());
			for (Shared_Ptr_Base *p : c) {
				elements.push_back(Element(p));
			}
		}

		// Restore pointers to the specified container.
		template <typename Container>
		void restore_to(Container &to) const {
			for (const Element &e : elements) {
				to.insert(e.pointer);
				if (e.count)
					e.count->ref();
			}
		}

	private:
		// Remember pointers on the stack, and the contents of the pointers at the time the set was
		// created.
		class Element {
		public:
			// Create.
			Element(Shared_Ptr_Base *ptr) : pointer(ptr), count(ptr->count) {
				if (count)
					count->ref();
			}

			// Copy.
			Element(const Element &o) : pointer(o.pointer), count(o.count) {
				if (count)
					count->ref();
			}

			Element &operator =(const Element &o) {
				if (o.count)
					o.count->ref();
				if (count)
					count->deref();

				pointer = o.pointer;
				count = o.count;

				return *this;
			}

			// Move.
			Element(Element &&o) : pointer(o.pointer), count(o.count) {
				o.pointer = nullptr;
				o.count = nullptr;
			}

			Element &operator =(Element &&o) {
				std::swap(pointer, o.pointer);
				std::swap(count, o.count);

				return *this;
			}

			// Location of the actual pointer on the stack.
			Shared_Ptr_Base *pointer;

			// Stored contents of the pointer, so that we can manipulate the reference count without
			// restoring the stack.
			Shared_Count *count;
		};

		// Array of elements.
		std::vector<Element> elements;
	};

}
