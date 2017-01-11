#pragma once

enum item_transfer_result_type {
	INVALID_SLOT_OR_UNOWNED_ROOT,

	/* returned by query_containment_result */
	NO_SLOT_AVAILABLE,

	INCOMPATIBLE_CATEGORIES,
	INSUFFICIENT_SPACE,
	THE_SAME_SLOT,

	SUCCESSFUL_TRANSFER,
	UNMOUNT_BEFOREHAND,
};