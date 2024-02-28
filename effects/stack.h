#pragma once
#include <ucontext.h>
#include <vector>

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

		// Friend the mirror to allow save/restore.
		friend class Stack_Mirror;
	};


	/**
	 * A copy of a stack. This class contains data that can be copied back onto a Stack in order to
	 * resume a continuation.
	 */
	class Stack_Mirror {
	public:
		// Create.
		Stack_Mirror(Handler_Frame *handler, Stack &original);

		// Associated handler frame.
		Handler_Frame *handler;

		// Restore.
		void restore() const;

	private:
		// Copy of the ucontext.
		ucontext_t context;

		// Contents of the stack.
		std::vector<char> stack_copy;

		// Stack we originally copied from, so that we can restore to it.
		Stack *original;
	};

}
