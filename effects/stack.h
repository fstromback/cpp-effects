#pragma once
#include <ucontext.h>

namespace effects {

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

		// Resume executing this stack. Does not save the current context anywhere.
		void resume();

	private:
		// The context that this stack represents. When the stack is currently being executed, the
		// contents of this member might not be reliable.
		ucontext_t context;

		// Pointer to the start of the allocated stack. Might be null.
		void *stack_base;

		// Size of the allocated stack.
		size_t stack_size;
	};


	/**
	 * A copy of a stack. This class contains data that can be copied back onto a Stack in order to
	 * resume a continuation.
	 */
	class Stack_Mirror {
	public:
	};

}
