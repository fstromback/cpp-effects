#pragma once
#include "handler_clause.h"
#include "handler_clause_decl.h"
#include <initializer_list>
#include <unordered_map>
#include <functional>
#include <list>

namespace effects {

	/**
	 * This file implements handlers for effects.
	 */

	// Clauses being handled. Effect ID -> handler.
	using Handler_Clause_Map = std::unordered_map<size_t, Handler_Clause *>;

	template <typename Result, typename Input>
	class Handler {
	public:
		Handler(const std::initializer_list<std::shared_ptr<Handler_Clause>> &clauses,
				std::function<Result (Input)> return_handler = [](Input x){ return x; })
			: return_handler(std::move(return_handler)) {

			for (auto &&clause : clauses) {
				this->unique_ptrs.push_back(clause);
				this->clauses.insert(std::make_pair(clause->id, this->unique_ptrs.back().get()));
			}
		}

		// Clauses being handled. Effect ID -> handler.
		Handler_Clause_Map clauses;

		// Return handler.
		std::function<Result (Input)> return_handler;

	private:
		// Store the unique ptrs in a vector.
		std::vector<std::shared_ptr<Handler_Clause>> unique_ptrs;
	};

}
