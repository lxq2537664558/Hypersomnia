#pragma once
#include "entity.h"
#include "world.h"
#include "component_bitset_matcher.h"

namespace augs {
	entity::entity(world& owner_world) : owner_world(owner_world) {}

	entity::~entity() {
		clear();
	}

	component_bitset_matcher entity::get_component_signature() {
#if USE_POINTER_TUPLE
		return signature;
#else
		std::vector<unsigned> indices;

		for (auto& raw : type_to_component.raw) {
			int index = owner_world.component_library.get_index(raw.key);
			indices.push_back(index);
		}

		return indices;
#endif
	}

	entity_id entity::get_id() {
		return owner_world.get_id_from_raw_pointer(this);
	}

	void entity::clear() {
#if USE_POINTER_TUPLE
		remove<components::animation>();
		remove<components::animation_response>();
		remove<components::behaviour_tree>();
		remove<components::camera>();
		remove<components::position_copying>();
		remove<components::children>();
		remove<components::crosshair>();
		remove<components::damage>();
		remove<components::gun>();
		remove<components::input>();
		remove<components::rotation_copying>();
		remove<components::movement>();
		remove<components::particle_emitter>();
		remove<components::particle_group>();
		remove<components::pathfinding>();
		remove<components::physics>();
		remove<components::render>();
		remove<components::steering>();
		remove<components::transform>();
		remove<components::visibility>();
		remove<components::sprite>();
		remove<components::polygon>();
		remove<components::tile_layer>();
		remove<components::car>();
		remove<components::driver>();
		remove<components::trigger>();
		remove<components::trigger_detector>();
		remove<components::fixtures>();
#else
		auto ids_to_remove = type_to_component.raw;

		for (auto c : ids_to_remove)
			remove(c.key);
#endif
	}

	void entity::remove(size_t component_type_hash) {
#if !USE_POINTER_TUPLE
		component_bitset_matcher old_signature(get_component_signature());

		component_bitset_matcher new_signature(old_signature);
		new_signature.remove(owner_world.component_library.get_index(component_type_hash));

		bool is_already_removed = old_signature == new_signature;

		if (is_already_removed)
			return;

		entity_id this_id = get_id();

		for (auto sys : owner_world.get_all_systems())
			/* if a processing_system does not match with the new signature and does with the old one */
			if (!sys->components_signature.matches(new_signature) && sys->components_signature.matches(old_signature))
				/* we should remove this entity from there */
				sys->remove(this_id);

		auto* component_ptr = type_to_component.get(component_type_hash);

		/* delete component from the corresponding pool, use hash to identify the proper destructor */
		owner_world.get_components_by_hash(component_type_hash).free_with_destructor(*component_ptr, component_type_hash);

		/* delete component entry from entity's map */
		type_to_component.remove(component_type_hash);

#ifdef INCLUDE_COMPONENT_NAMES
		typestrs.remove(component_type_hash);
#endif
#else
		assert(0);
#endif
	}
}