#pragma once

enum class containment_result_type {
	INVALID_RESULT,
	THE_SAME_SLOT,
	TOO_MANY_ITEMS,
	INCOMPATIBLE_CATEGORIES,
	INSUFFICIENT_SPACE,

	SUCCESSFUL_REPLACE,
	SUCCESSFUL_CONTAINMENT
};

enum class item_transfer_result_type {
	INVALID_RESULT,

	INVALID_CAPABILITIES,

	/* returned by query_containment_result */
	TOO_MANY_ITEMS,

	INCOMPATIBLE_CATEGORIES,
	INSUFFICIENT_SPACE,
	THE_SAME_SLOT,

	SUCCESSFUL_TRANSFER,
	SUCCESSFUL_PICKUP,
	SUCCESSFUL_DROP
};

bool is_successful(const item_transfer_result_type t);