#include "handler_frame.h"
#include "handle.h"
#include <cassert>

namespace effects {

#define THREAD __thread

	// Per-thread link to a handler frame.
	THREAD Handler_Frame *top_handler = nullptr;

	// Get the current one.
	Handler_Frame *Handler_Frame::current() {
		if (!top_handler) {
			top_handler = new Handler_Frame(Stack::current);
		}
		return top_handler;
	}

	Handler_Frame::Handler_Frame(Stack::Create create_mode)
		: stack(create_mode), previous(nullptr) {}

	void Handler_Frame::call(Handle_Body *body, const Handler_Clause_Map &clauses) {
		Handler_Frame *current = Handler_Frame::current();
		Handler_Frame *next = new Handler_Frame(Stack::allocate);
		next->previous = current;
		next->clauses = clauses; // TODO: Do we really want to copy?
		top_handler = next;

		// Execute the stack!
		next->stack.start(current->stack, &frame_main, body);

		// Note: We will return here after execution is complete, or when an effect was triggered.
		while (current->to_resume.effect) {
			Resume_Params resume = current->to_resume;
			current->to_resume = Resume_Params();

			// TODO: Create a continuation based on where we resumed from.

			// Resume!
			resume.result_to = body;
			resume.call(Continuation_Base());
		}
	}

	void Handler_Frame::frame_main(void *b) {
		Handle_Body *body = reinterpret_cast<Handle_Body *>(b);
		body->call();

		// Unlink:
		Handler_Frame *current = top_handler;
		top_handler = current->previous;

		// TODO: Refcount instead?
		delete current;
	}

	void Handler_Frame::call_handler(size_t id, Captured_Effect *captured) {
		for (Handler_Frame *current = top_handler; current; current = current->previous) {
			auto found = current->clauses.find(id);
			if (found != current->clauses.end()) {
				Handler_Frame *prev = current->previous;
				assert(prev);
				prev->call_handler(*found->second, captured);
				return;
			}
		}
		throw no_handler();
	}

	void Handler_Frame::call_handler(const Handler_Clause &clause, Captured_Effect *captured) {
		assert(to_resume.effect == nullptr);
		to_resume.effect = captured;
		to_resume.to_call = &clause;

		// TODO: Memcpy all stacks to create a continuation and store it somewhere!
		// We need to do that once we are back on the original stack.

		stack.resume();
	}
}
