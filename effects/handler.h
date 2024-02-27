#pragma once
#include "handler_clause.h"
#include "effect.h"
#include <initializer_list>
#include <unordered_map>
#include <functional>
#include <list>
#include <memory>

namespace effects {

	/**
	 * This file implements handlers for effects.
	 */

	// Clauses being handled. Effect ID -> handler.
	using Handler_Clause_Map = std::unordered_map<size_t, Handler_Clause *>;

	// Helper class for the initializer list.
	template <typename T>
	class Handler_Init {
	public:
		template <typename Signature, typename Body>
		Handler_Init(const Effect<Signature> &effect, Body &&body)
			: ptr(std::make_shared<Bound_Handler_Clause<T, Signature>>(effect.id(), std::forward<Body>(body))) {}

		std::shared_ptr<Handler_Clause> ptr;
	};

	template <typename Result, typename Input>
	class Handler {
	public:
		// Handler(const std::initializer_list<std::shared_ptr<Partial_Handler_Clause<Input>>> &clauses,
		// 		std::function<Result (Input)> return_handler = [](Input x){ return x; })
		Handler(const std::initializer_list<Handler_Init<Input>> &clauses,
				std::function<Result (Input)> return_handler = [](Input x){ return x; })
			: return_handler(std::move(return_handler)) {

			for (auto &&clause : clauses) {
				this->unique_ptrs.push_back(std::move(clause.ptr));
				this->clauses.insert(std::make_pair(clause.ptr->id, this->unique_ptrs.back().get()));
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
