#include "handler_frame.h"
#include "handle.h"

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

	void Handler_Frame::call(Handler_Body *body, const Handler_Clause_Map &clauses) {
		Handler_Frame *frame = new Handler_Frame(Stack::allocate);
		frame->previous = current();
		frame->clauses = clauses; // TODO: Do we really want to copy?

		// Execute the stack!
		frame->stack.start(frame->previous->stack, &frame_main, body);

		// Note: We will return here after execution is complete, or when an effect was triggered.
	}

	void Handler_Frame::frame_main(void *b) {
		Handler_Body *body = reinterpret_cast<Handler_Body *>(b);
		body->call();

		// Unlink:
		Handler_Frame *current = top_handler;
		top_handler = current->previous;

		// TODO: Refcount instead?
		delete current;
	}

}
