#pragma once
#include "handler_clause.h"
#include <initializer_list>
#include <unordered_map>
#include <functional>

namespace effects {

	/**
	 * This file implements handlers for effects.
	 */

	// Clauses being handled. Effect ID -> handler.
	using Handler_Clause_Map = std::unordered_map<size_t, Handler_Clause>;

	template <typename Result, typename Input>
	class Handler {
	public:
		Handler(const std::initializer_list<Handler_Clause> &clauses,
				std::function<Result (Input)> return_handler = [](Input x){ return x; })
			: return_handler(std::move(return_handler)) {

			for (auto &&clause : clauses) {
				this->clauses.insert(std::make_pair(clause.id, std::move(clause)));
			}
		}

		// Clauses being handled. Effect ID -> handler.
		Handler_Clause_Map clauses;

		// Return handler.
		std::function<Result (Input)> return_handler;
	};

}
