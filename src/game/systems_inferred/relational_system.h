#pragma once
#include "augs/misc/children_vector_tracker.h"
#include "game/transcendental/entity_id.h"
#include "game/transcendental/entity_handle_declaration.h"

namespace components {
	struct fixtures;
}

struct cosmos_global_state;

template <bool, class>
class component_synchronizer;

class relational_system {
	friend class cosmos;
	friend class physics_system;

	friend class component_synchronizer<false, components::fixtures>;

	template <bool is_const>
	friend class basic_physics_synchronizer;

	void reserve_caches_for_entities(const size_t n);
	void create_inferred_state_for(const const_entity_handle);
	void destroy_inferred_state_of(const const_entity_handle);

	void create_additional_inferred_state(const cosmos_global_state&) {}
	void destroy_additional_inferred_state(const cosmos_global_state&) {}

	void handle_deletion_of_potential_parent(const entity_id);

	augs::children_vector_tracker<unversioned_entity_id, 1> fixtures_of_bodies;
	augs::children_vector_tracker<unversioned_entity_id, 2> joints_of_bodies;

	template <class F>
	void for_each_tracker(F f) {
		f(fixtures_of_bodies);
		f(joints_of_bodies);
	}
};