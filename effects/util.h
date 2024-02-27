#pragma once
#include <tuple>

namespace effects {

	/**
	 * General utilities.
	 */

	// Helper to call a function with parameters in a tuple.
	template <typename Result, typename Tuple>
	struct Tuple_Call {
		template <int N, int... S>
		struct Arg_Seq : Arg_Seq<N - 1, N - 1, S...> {};

		template <int... S>
		struct Arg_Seq<0, S...> {
			template <typename Function>
			static Result call(Function &&fn, const Tuple &args) {
				return fn(std::get<S>(args)...);
			}

			template <typename Function, typename Append>
			static Result call(Function &&fn, const Tuple &args, Append &&append) {
				return fn(std::get<S>(args)..., std::forward<Append>(append));
			}
		};

		template <typename Function>
		static Result call(Function &&fn, const Tuple &args) {
			return Arg_Seq<std::tuple_size<Tuple>::value>::call(fn, args);
		}

		template <typename Function, typename Append>
		static Result call(Function &&fn, const Tuple &args, Append &&append) {
			return Arg_Seq<std::tuple_size<Tuple>::value>::call(fn, args, std::forward<Append>(append));
		}
	};

}
