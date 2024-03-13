#include "stack.h"
#include "debug.h"
#include <unistd.h>
#include <sys/mman.h>
#include <new>

namespace effects {

	// Size of a stack:
	static const size_t stack_size = 1 * 1024 * 1024; // 1 MiB

	// Get the current page size.
	static size_t page_size() {
		static size_t sz = 0;
		if (sz == 0) {
			int s = getpagesize();
			sz = static_cast<size_t>(s);
		}
		return sz;
	}


	Stack::Stack(Create mode) : stack_base(nullptr), stack_size(0) {
		getcontext(&context);

		if (mode == allocate) {
			size_t page_size = effects::page_size();
			size_t size = ((effects::stack_size + page_size - 1) / page_size) * page_size; // Round up.
			size += page_size; // Guard page.

			void *memory = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			if (!memory)
				throw std::bad_alloc();

			mprotect(memory, 1, PROT_NONE); // For the guard page.

			this->stack_base = static_cast<char *>(memory) + page_size;
			this->stack_size = size - page_size;

			context.uc_stack.ss_sp = stack_base;
			context.uc_stack.ss_size = stack_size;
		}
	}

	Stack::~Stack() {
		if (!stack_base) {
			size_t page_size = effects::page_size();
			void *base = static_cast<char *>(stack_base) - page_size;
			munmap(base, stack_size + page_size);
		}
	}

	void Stack::start(Stack &prev, void (*fn)(void *), void *param) {
		context.uc_link = &prev.context;
		makecontext(&context, reinterpret_cast<void (*)()>(fn), 1, param);
		swapcontext(&prev.context, &context);
	}

	void Stack::resume(Stack &prev) {
		swapcontext(&prev.context, &context);
	}

	std::ostream &operator <<(std::ostream &to, const Stack &s) {
		void *end = reinterpret_cast<char *>(s.stack_base) + s.stack_size;
		return to << "Stack: " << s.stack_base << " - " << end;
	}

	static size_t get_sp(const ucontext_t &context) {
#if defined(__linux__)

#if defined(__x86_64__)
		return context.uc_mcontext.gregs[REG_RSP];
#elif defined(__aarch64__)
		return context.uc_mcontext.sp;
#else
#error "Unknown machine, can not extract the stack pointer."
		return 0;
#endif

#elif defined(__APPLE__)

		// Note: The mcontext is a pointer, we probably need to store that as well!
#if defined(__x86_64)
#if __DARWIN_UNIX03
		return context.uc_mcontext->__ss.__rsp;
#else
		return context.uc_mcontext->ss.rsp;
#endif
#elif defined(__aarch64__)
#if __DARWIN_UNIX03
		return context.uc_mcontext->__ss.__sp;
#else
		return context.uc_mcontext->ss.sp;
#endif
#else
#error "Unknown machine, can not extract the stack pointer."
		return 0;
#endif

#else
#error "Unknown platform."
#endif
	}

	template <typename T>
	struct Save_MContext {
		static void save(T &context, std::vector<char> &store) {
			// Only need to do something if the member is a pointer!
			(void)context;
			(void)store;
		}
	};

	template <typename T>
	struct Save_MContext<T *> {
		// Only need to do something if the member is a pointer!
		static void save(T *&context, std::vector<char> &store) {
			const char *begin = reinterpret_cast<const char *>(context);
			const char *end = begin + sizeof(T);
			store = std::vector<char>(begin, end);

			// Store the updated pointer back!
			context = reinterpret_cast<T *>(store.data());
		}
	};

	static void save_mcontext(ucontext_t &context, std::vector<char> &store) {
		using Type = std::remove_cv_t<std::remove_reference_t<decltype(context.uc_mcontext)>>;
		Save_MContext<Type>::save(context.uc_mcontext, store);
	}

	Stack_Mirror::Stack_Mirror(Stack &src, Pointer_Set ptrs, Shared_Ptr<Handler_Frame> handler)
		: handler(std::move(handler)), shared_ptrs(std::move(ptrs)), original(&src) {

		context = src.context;

		size_t stack_low = reinterpret_cast<size_t>(context.uc_stack.ss_sp);
		size_t stack_high = stack_low + context.uc_stack.ss_size;

		size_t sp = get_sp(context);

		// Save the machine context if we need to do that.
		save_mcontext(context, state_copy);

		// Note: We assume that stack grows towards lower adresses.
		char *copy_start = reinterpret_cast<char *>(sp);
		size_t to_copy = stack_high - sp;
		stack_copy = std::vector<char>(copy_start, copy_start + to_copy);
	}

	void Stack_Mirror::restore() const {
		original->context = context;

		size_t stack_low = reinterpret_cast<size_t>(context.uc_stack.ss_sp);
		size_t stack_high = stack_low + context.uc_stack.ss_size;

		char *copy_to = reinterpret_cast<char *>(stack_high - stack_copy.size());
		std::copy(stack_copy.begin(), stack_copy.end(), copy_to);
	}

}
