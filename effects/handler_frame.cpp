#include "handler_frame.h"
#include "handle.h"
#include <cassert>

namespace effects {

	// Per-thread link to a handler frame.
	thread_local Shared_Ptr<Handler_Frame> top_handler;

	// Get the current one.
	Shared_Ptr<Handler_Frame> Handler_Frame::current() {
		if (!top_handler) {
			top_handler = mk_shared<Handler_Frame>(Stack::current);
		}
		return top_handler;
	}

	Handler_Frame::Handler_Frame(Stack::Create create_mode)
		: stack(create_mode), previous() {}

	Handler_Frame::~Handler_Frame() {
		if (!shared_ptrs.empty()) {
			PLN("WARNING: Shared pointers are still alive in a handler frame!");
			for (Shared_Ptr_Base *p : shared_ptrs) {
				PLN("  Pointer at " << p << ", count-object " << *(void **)p);
			}
		}
	}

	void Handler_Frame::call(const Shared_Ptr<Handle_Body> &body, const Handler_Clause_Map &clauses) {
		Shared_Ptr<Handler_Frame> current = Handler_Frame::current();
		Shared_Ptr<Handler_Frame> next = mk_shared<Handler_Frame>(Stack::allocate);

		// Note: Creation of Shared_Ptr above must be before these lines!
		next->previous = current;
		next->clauses = clauses; // TODO: Do we really want to copy?
		top_handler = next;

		// Execute the stack!
		next->stack.start(current->stack, &frame_main, body.get());

		// Here, it is once again safe to allocate/deallocate Shared_Ptrs since frame_main has
		// restored "top_frame" at this point.

		// Note: We will return here after execution is complete, or when an effect was triggered.
		while (current->to_resume.effect) {
			Resume resume = current->to_resume;
			current->to_resume = Resume();

			// Capture the continuation and reset the top handler.
			Captured_Continuation continuation = capture_continuation(top_handler, current);
			top_handler = current;

			// Resume!
			Resume_Params params = {
				body.get(),
				resume.to_call,
				&continuation
			};
			resume.effect->call(params);
		}
	}

	void Handler_Frame::frame_main(void *b) {
		Handle_Body *body = reinterpret_cast<Handle_Body *>(b);

		body->call();

		// Unlink:
		Shared_Ptr<Handler_Frame> current = top_handler;
		top_handler = current->previous;

		// Since we messed with the linking here, we need to manually unregister "current":
		current->shared_ptrs.erase(&current);
	}

	void Handler_Frame::call_handler(size_t id, Captured_Effect *captured) {
		for (Handler_Frame *current = top_handler.get(); current; current = current->previous.get()) {
			auto found = current->clauses.find(id);
			if (found != current->clauses.end()) {
				Handler_Frame *prev = current->previous.get();
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

		stack.resume(top_handler->stack);
	}

	Captured_Continuation Handler_Frame::capture_continuation(
		const Shared_Ptr<Handler_Frame> &from,
		const Shared_Ptr<Handler_Frame> &to) {

		size_t depth = 0;
		for (Shared_Ptr<Handler_Frame> current = from; current != to; current = current->previous) {
			depth++;
		}

		Captured_Continuation captured(depth);

		size_t id = 0;
		for (Shared_Ptr<Handler_Frame> current = from; current != to; current = current->previous, id++) {
			captured.frames.push_back(Stack_Mirror(current->stack, Pointer_Set(current->shared_ptrs), current));
		}

		// Copy elision.
		return captured;
	}

	void Handler_Frame::resume_continuation(const Captured_Continuation &src) {
		Stack &save_to = top_handler->stack;

		// Link the handlers into "top_frame". Also update reference counts.
		for (size_t i = src.frames.size(); i > 0; i--) {
			const Stack_Mirror &mirror = src.frames[i - 1];
			const Shared_Ptr<Handler_Frame> &handler = mirror.handler;

			// Link into the top frame.
			handler->previous = top_handler;
			top_handler = handler;

			// Update ref-counts.
			handler->shared_ptrs.clear();
			mirror.shared_ptrs.restore_to(handler->shared_ptrs);
		}

		// Finally, resume the topmost one:
		top_handler->stack.resume(save_to);
	}

	void Handler_Frame::add_shared_ptr(Shared_Ptr_Base *p) {
		// Note: It is OK to use top_handler directly, since we are only interested in storing
		// pointers for child frames, never for the root.
		Handler_Frame *c = top_handler.get();
		if (c && c->stack.contains(p))
			c->shared_ptrs.insert(p);
	}

	void Handler_Frame::remove_shared_ptr(Shared_Ptr_Base *p) {
		Handler_Frame *c = top_handler.get();
		if (c && c->stack.contains(p))
			c->shared_ptrs.erase(p);
	}

}
