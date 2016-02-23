#pragma once

enum item_category {
	//ITEM = 1 << 0,
	RAIL_ATTACHMENT = 1 << 1,
	BARREL_ATTACHMENT = 1 << 2,
	SHOULDER_CONTAINER = 1 << 3,
	MAGAZINE = 1 << 4,
	SHOT_CHARGE = 1 << 5,
};

enum class slot_function {
	ITEM_DEPOSIT,

	GUN_CHAMBER,
	GUN_DETACHABLE_MAGAZINE,
	GUN_CHAMBER_MAGAZINE,
	GUN_RAIL,
	GUN_SHOULDER,

	PRIMARY_HAND,
	SECONDARY_HAND,
	
	SHOULDER_SLOT,
	TORSO_ARMOR_SLOT
};