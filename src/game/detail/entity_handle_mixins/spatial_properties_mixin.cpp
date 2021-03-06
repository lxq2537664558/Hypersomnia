#include "game/transcendental/entity_handle.h"
#include "game/transcendental/cosmos.h"

#include "game/components/rigid_body_component.h"
#include "game/components/special_physics_component.h"
#include "game/components/fixtures_component.h"
#include "game/components/wandering_pixels_component.h"
#include "game/components/position_copying_component.h"
#include "game/components/interpolation_component.h"
#include "game/components/item_component.h"

#include "game/components/transform_component.h"
#include "game/components/all_inferred_state_component.h"

#include "spatial_properties_mixin.h"
#include "game/systems_audiovisual/interpolation_system.h"
#include "augs/graphics/drawers.h"

template <bool C, class D>
bool basic_spatial_properties_mixin<C, D>::has_logic_transform() const {
	const auto handle = *static_cast<const D*>(this);
	const auto owner = handle.get_owner_body();
	
	if (owner.alive() && owner != handle) {
		return true;
	}
	else if (handle.has<components::rigid_body>()) {
		return true;
	}
	else if (handle.has<components::transform>()) {
		return true;
	}
	else if (handle.has<components::wandering_pixels>()) {
		return true;
	}

	return false;
}

template <bool C, class D>
components::transform basic_spatial_properties_mixin<C, D>::get_logic_transform() const {
	const auto handle = *static_cast<const D*>(this);

	const auto owner = handle.get_owner_body();

	if (owner.alive()) {
		ensure(!handle.has<components::transform>());

		const auto& phys = owner.get<components::rigid_body>();

		if (owner != handle) {
			const auto& fixtures = handle.get<components::fixtures>();

			if (fixtures.is_activated() && phys.is_activated()) {
				return components::fixtures::transform_around_body(handle, owner.get_logic_transform());
			}
			else {
				ensure(handle.has<components::item>());

				return handle.get_current_slot().get_container().get_logic_transform();
			}
		}
		else {
			if (phys.is_activated()) {
				return{ phys.get_position(), phys.get_angle() };
			}
			else {
				ensure(handle.has<components::item>());

				return handle.get_current_slot().get_container().get_logic_transform();
			}
		}
	}
	else if (handle.has<components::wandering_pixels>()) {
		return handle.get<components::wandering_pixels>().reach.center();
	}
	else {
		return handle.get<components::transform>();
	}	
}

template <bool C, class D>
vec2 basic_spatial_properties_mixin<C, D>::get_effective_velocity() const {
	const auto handle = *static_cast<const D*>(this);
	const auto owner = handle.get_owner_body();

	if (owner.alive()) {
		return owner.get<components::rigid_body>().velocity();
	}
	else if (handle.has<components::position_copying>()) {
		ensure(handle.has<components::transform>());
		return 
			(handle.get<components::transform>().pos - handle.get<components::position_copying>().get_previous_transform().pos)
			/ static_cast<float>(handle.get_cosmos().get_fixed_delta().in_seconds());
	}
	
	return{};
}

template <bool C, class D>
components::transform basic_spatial_properties_mixin<C, D>::get_viewing_transform(const interpolation_system& sys, const bool integerize) const {
	const auto handle = *static_cast<const D*>(this);

	const auto& owner = handle.get_owner_body();

	if (owner.alive() && owner.has<components::interpolation>() && owner != handle) {
		auto in = sys.get_interpolated(owner);

		if (integerize) {
			in.pos.discard_fract();
		}

		return components::fixtures::transform_around_body(handle, in);
	}
	else if (handle.has<components::interpolation>()) {
		return sys.get_interpolated(handle);
	}

	return handle.get_logic_transform();
}

template <class D>
void spatial_properties_mixin<false, D>::set_logic_transform(
	const logic_step step,
	const components::transform t
) const {
	if (get_logic_transform() == t) {
		return;
	}
	
	const auto handle = *static_cast<const D*>(this);
	const auto owner_body = handle.get_owner_body();

	const bool this_entity_does_not_have_its_own_transform = 
		owner_body.alive() 
		&& owner_body != handle
	;

	ensure(!this_entity_does_not_have_its_own_transform);
	
	const auto rigid_body = handle.find<components::rigid_body>();

	if (rigid_body != nullptr) {
		ensure(!handle.has<components::transform>());
		rigid_body.set_transform(t);
	}
	else {
		handle.get<components::transform>() = t;

		if (handle.has<components::tree_of_npo_node>()) {
			handle.get<components::tree_of_npo_node>().update_proxy(step);
		}
	}
}

// explicit instantiation
template class spatial_properties_mixin<false, basic_entity_handle<false>>;
template class spatial_properties_mixin<true, basic_entity_handle<true>>;
template class basic_spatial_properties_mixin<false, basic_entity_handle<false>>;
template class basic_spatial_properties_mixin<true, basic_entity_handle<true>>;