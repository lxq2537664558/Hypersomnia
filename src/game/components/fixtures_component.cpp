#include <numeric>

#include <3rdparty/Box2D/Dynamics/b2Fixture.h>
#include <3rdparty/Box2D/Box2D.h>

#include "augs/ensure.h"

#include "game/transcendental/cosmos.h"
#include "game/transcendental/entity_handle.h"

#include "game/components/fixtures_component.h"
#include "game/components/all_inferred_state_component.h"
#include "game/components/rigid_body_component.h"

#include "game/detail/physics/b2Fixture_index_in_component.h"

typedef components::fixtures F;

template<bool C>
maybe_const_ref_t<C, colliders_cache>& basic_fixtures_synchronizer<C>::get_cache() const {
	auto& cosmos = handle.get_cosmos();
	return cosmos.systems_inferred.get<physics_system>().get_colliders_cache(handle);
}

template<bool C>
ltrb basic_fixtures_synchronizer<C>::get_local_aabb() const {
	ensure("Not implemented" && false);
	return {};
	//std::vector<vec2> all_verts;
	//
	//for (const auto& s : get_raw_component().colliders) {
	//	for (const auto& c : s.shape.convex_polys) {
	//		for (const auto v : c.vertices) {
	//			all_verts.push_back(v);
	//		}
	//	}
	//}
	//
	//return augs::get_aabb(all_verts);
}

template<bool C>
bool basic_fixtures_synchronizer<C>::is_friction_ground() const {
	return get_raw_component().is_friction_ground;
}

template<bool C>
bool basic_fixtures_synchronizer<C>::is_destructible() const {
	return get_raw_component().destructible;
}

template<bool C>
bool basic_fixtures_synchronizer<C>::standard_collision_resolution_disabled() const {
	return get_raw_component().disable_standard_collision_resolution;
}

template<bool C>
bool basic_fixtures_synchronizer<C>::can_driver_shoot_through() const {
	return get_raw_component().can_driver_shoot_through;
}

void component_synchronizer<false, F>::set_offset(
	const colliders_offset_type t, 
	const components::transform off
) const {
	get_raw_component().offsets_for_created_shapes[t] = off;
	reinference();
}

const component_synchronizer<false, F>& component_synchronizer<false, F>::operator=(const F& f) const {
	set_owner_body(f.owner_body);
	get_raw_component() = f;
	reinference();
	return *this;
}

void component_synchronizer<false, F>::reinference() const {
	handle.get_cosmos().partial_reinference<relational_system>(handle);
	handle.get_cosmos().partial_reinference<physics_system>(handle);
}

void component_synchronizer<false, F>::rebuild_density() const {
	for (auto f : get_cache().all_fixtures_in_component) {
		f->SetDensity(get_raw_component().density * get_raw_component().density_multiplier);
	}

	get_cache().all_fixtures_in_component[0]->GetBody()->ResetMassData();
}

void component_synchronizer<false, F>::set_density(
	const float d
) const {
	get_raw_component().density = d;

	if (!is_constructed()) {
		return;
	}

	rebuild_density();
}

void component_synchronizer<false, F>::set_density_multiplier(
	const float mult
) const {
	get_raw_component().density_multiplier = mult;

	if (!is_constructed()) {
		return;
	}

	rebuild_density();
}

void component_synchronizer<false, F>::set_activated(const bool flag) const {
	if (flag == get_raw_component().activated) {
		return;
	}

	get_raw_component().activated = flag;
	reinference();
}

void component_synchronizer<false, F>::set_friction(
	const float fr
) const {
	get_raw_component().friction = fr;

	if (!is_constructed()) {
		return;
	}

	for (auto f : get_cache().all_fixtures_in_component) {
		f->SetFriction(fr);
	}
}

void component_synchronizer<false, F>::set_restitution(
	const float r
) const {
	get_raw_component().restitution = r;

	if (!is_constructed()) {
		return;
	}

	for (auto f : get_cache().all_fixtures_in_component) {
		f->SetRestitution(r);
	}
}

void component_synchronizer<false, F>::set_physical_material(
	const assets::physical_material_id m
) const {
	get_raw_component().material = m;
}

void component_synchronizer<false, F>::set_owner_body(const entity_id owner_id) const {
	const auto self = handle;

	auto& cosmos = self.get_cosmos();
	const auto new_owner = cosmos[owner_id];
	const auto this_id = self.get_id();

	const auto former_owner = cosmos[get_raw_component().owner_body];
	get_raw_component().owner_body = new_owner;

	auto& relational = cosmos.systems_inferred.get<relational_system>().fixtures_of_bodies;
	relational.set_parent(self, new_owner);

	if (former_owner.alive()) {
		cosmos.partial_reinference<physics_system>(former_owner);
	}

	ensure(new_owner.alive());
	cosmos.partial_reinference<physics_system>(new_owner);
}

template<bool C>
float basic_fixtures_synchronizer<C>::get_friction() const {
	return get_raw_component().friction;
}

template<bool C>
float basic_fixtures_synchronizer<C>::get_restitution() const {
	return get_raw_component().restitution;
}

template<bool C>
float basic_fixtures_synchronizer<C>::get_density() const {
	return get_base_density() * get_raw_component().density_multiplier;
}

template<bool C>
float basic_fixtures_synchronizer<C>::get_base_density() const {
	return get_raw_component().density;
}

template<bool C>
float basic_fixtures_synchronizer<C>::get_density_multiplier() const {
	return get_raw_component().density_multiplier;
}

template<bool C>
bool basic_fixtures_synchronizer<C>::is_activated() const {
	return get_raw_component().activated;
}

template<bool C>
bool basic_fixtures_synchronizer<C>::is_constructed() const {
	return handle.get_cosmos().systems_inferred.get<physics_system>().is_inferred_state_created_for_colliders(handle);
}

template<bool C>
components::transform basic_fixtures_synchronizer<C>::get_offset(const colliders_offset_type t) const {
	return get_raw_component().offsets_for_created_shapes[t];
}

template<bool C>
components::transform basic_fixtures_synchronizer<C>::get_total_offset() const {
	const auto& offsets = get_raw_component().offsets_for_created_shapes;

	return std::accumulate(
		offsets.begin(), 
		offsets.end(), 
		components::transform()
	);
}

components::transform components::fixtures::transform_around_body(
	const const_entity_handle fe, 
	const components::transform body_transform
) {
	const auto total_offset = fe.get<components::fixtures>().get_total_offset();
	
	components::transform displaced = body_transform + total_offset;
	displaced.pos.rotate(body_transform.rotation, body_transform.pos);

	return displaced;
}

template<bool C>
entity_id basic_fixtures_synchronizer<C>::get_owner_body() const {
	return get_raw_component().owner_body;
}

template class basic_fixtures_synchronizer<false>;
template class basic_fixtures_synchronizer<true>;