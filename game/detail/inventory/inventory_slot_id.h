#pragma once
#include "game/enums/slot_function.h"
#include "game/transcendental/entity_id.h"

#include "augs/templates/hash_templates.h"
#include "augs/misc/constant_size_vector.h"

#include "game/components/transform_component.h"

template <class id_type>
struct basic_inventory_slot_id {
	slot_function type;
	id_type container_entity;

	basic_inventory_slot_id();
	basic_inventory_slot_id(const slot_function, const id_type);

	template <class Archive>
	void serialize(Archive& ar) {
		ar(
			CEREAL_NVP(type),
			CEREAL_NVP(container_entity)
		);
	}

	void unset();

	bool operator<(const basic_inventory_slot_id b) const;
	bool operator==(const basic_inventory_slot_id b) const;
	bool operator!=(const basic_inventory_slot_id b) const;
};

typedef basic_inventory_slot_id<entity_id> inventory_slot_id;

struct inventory_item_address {
	entity_id root_container;
	augs::constant_size_vector<slot_function, 12> directions;
};

struct inventory_traversal {
	inventory_slot_id parent_slot;
	inventory_item_address current_address;
	components::transform attachment_offset;
	bool item_remains_physical = true;
};

namespace std {
	template <>
	struct hash<inventory_slot_id> {
		std::size_t operator()(const inventory_slot_id k) const {
			return augs::simple_two_hash(k.container_entity, k.type);
		}
	};
}