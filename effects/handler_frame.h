#pragma once
#include "stack.h"
#include "handler.h"
#include "captured_effect.h"
#include "pointer.h"
#include <unordered_set>

namespace effects {

	class Handle_Body;
	class Handler_Clause;
	class Shared_Ptr_Base;

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

		// Destroy. Mostly for sanity-checking.
		~Handler_Frame();

		// Get the current handler frame for the current thread.
		static Handler_Frame *current();

		// Call a function on a new handler frame.
		static void call(Handle_Body *body, const Handler_Clause_Map &clauses);

		// Call an effect handler.
		static void call_handler(size_t id, Captured_Effect *captured);

		// Resume a continuation. Assumes that all stacks in 'cont' have been restored previously.
		static void resume_continuation(const Captured_Continuation &cont);

	private:
		// Stack that this frame executes on.
		Stack stack;

		// List of shared pointers allocated on the stack.
		std::unordered_set<Shared_Ptr_Base *> shared_ptrs;

		// Previous frame, if any.
		Handler_Frame *previous;

		// Clauses handled here.
		Handler_Clause_Map clauses;

		/**
		 * Data structure used to determine what to resume.
		 */
		struct Resume {
			// Effect to execute.
			Captured_Effect *effect = nullptr;

			// Handler clause to call.
			const Handler_Clause *to_call = nullptr;
		};

		// Effect handler to resume.
		Resume to_resume;

		// Helper function used as the "main" function for new handler frames.
		static void frame_main(void *ptr);

		// Helper to actually call the handler we found.
		void call_handler(const Handler_Clause &clause, Captured_Effect *captured);

		// Helper to capture a continuation.
		static Captured_Continuation capture_continuation(Handler_Frame *from, Handler_Frame *to);

		// Allow registering shared ptrs here.
		friend class Shared_Ptr_Base;

		// Add/remove shared pointers.
		static void add_shared_ptr(Shared_Ptr_Base *p);
		static void remove_shared_ptr(Shared_Ptr_Base *p);
	};

}
