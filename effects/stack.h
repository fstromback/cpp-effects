#pragma once
#include <ucontext.h>
#include <vector>
#include <iostream>
#include "pointer_set.h"

namespace effects {

	class Handler_Frame;

	/**
	 * This file contains logic for manipulation of different execution stacks on a thread.
	 */


	/**
	 * A stack that is allocated and ready to be used as the execution stack of a thread.
	 */
	class Stack {
	public:
		// How to create a new stack.
		enum Create {
			current, // Create a mirror of the current stack.
			allocate, // Allocate a new stack.
		};

		// Create a new stack, but do not execute it yet.
		Stack(Create mode);

		// Destroy.
		~Stack();

		Stack(const Stack &) = delete;
		Stack &operator =(const Stack &) = delete;

		// Start running this stack from the specified function. Will resume executing "prev" after
		// the function executes to completion.
		void start(Stack &prev, void (*func)(void *), void *arg);

		// Resume executing this stack. Saves the current stack in "prev".
		void resume(Stack &prev);

		// Does the stack contain an object?
		bool contains(void *ptr) const {
			size_t start = reinterpret_cast<size_t>(stack_base);
			size_t p = reinterpret_cast<size_t>(ptr);
			return p >= start && p < (start + stack_size);
		}

	private:
		// The context that this stack represents. When the stack is currently being executed, the
		// contents of this member might not be reliable.
		ucontext_t context;

		// Pointer to the start of the allocated stack. Might be null.
		void *stack_base;

		// Size of the allocated stack.
		size_t stack_size;

		// Friend the mirror to allow save/restore.
		friend class Stack_Mirror;

		// For ouput.
		friend std::ostream &operator <<(std::ostream &to, const Stack &s);
	};

	// Output.
	std::ostream &operator <<(std::ostream &to, const Stack &s);


	/**
	 * A copy of a stack. This class contains data that can be copied back onto a Stack in order to
	 * resume a continuation.
	 */
	class Stack_Mirror {
	public:
		// Create.
		Stack_Mirror(Stack &original, Pointer_Set shared_ptrs, Shared_Ptr<Handler_Frame> handler);

		// Associated handler frame.
		Shared_Ptr<Handler_Frame> handler;

		// Pointers stored on the stack.
		Pointer_Set shared_ptrs;

		// Restore.
		void restore() const;

	private:
		// Copy of the ucontext.
		ucontext_t context;

		// Contents of the stack.
		std::vector<char> stack_copy;

		// Copy of the machine state, if not included in "context" above on this particular platform.
		std::vector<char> state_copy;

		// Stack we originally copied from, so that we can restore to it.
		Stack *original;
	};

}
