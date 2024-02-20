#pragma once
#include "stack.h"
#include "handler.h"

namespace effects {

	class Handler_Body;
	class Handler_Clause;

	/**
	 * Represents a handler frame on the current execution stack.
	 *
	 * A handler frame always represents a transition from one execution stack to another. The
	 * exception is the first frame of each thread. It represents a frame that corresponds to the
	 * first, non-handler frame.
	 *
	 * TODO: In these frames, we could store ref-counted objects that need to be updated when
	 * continuations are resumed. This would make it possible to implement an effects::shared_ptr
	 * that is safe in the context of continuations, and we could then use that to heap-allocate
	 * most data structures we need.
	 */
	class Handler_Frame {
	public:
		// Create the frame. The parameter is forwarded to the constructor of the Stack member to
		// determine how it is created.
		Handler_Frame(Stack::Create stack_mode);

		// Get the current handler frame for the current thread.
		static Handler_Frame *current();

		// Call a function on a new handler frame.
		static void call(Handler_Body *body, const Handler_Clause_Map &clauses);

		// Find a handler for an effect.
		static Handler_Clause *find(size_t effect_id);

	private:
		// Stack that this frame executes on.
		Stack stack;

		// Previous frame, if any.
		Handler_Frame *previous;

		// Clauses handled here.
		Handler_Clause_Map clauses;

		// Helper function used as the "main" function for new handler frames.
		static void frame_main(void *ptr);
	};

}
