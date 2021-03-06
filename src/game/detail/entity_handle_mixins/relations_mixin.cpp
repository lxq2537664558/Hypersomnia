#include "relations_mixin.h"
#include "game/detail/inventory/inventory_slot_id.h"
#include "game/detail/inventory/inventory_slot_handle.h"
#include "game/transcendental/entity_handle.h"
#include "game/transcendental/cosmos.h"
#include "game/transcendental/types_specification/all_component_includes.h"
#include "game/components/all_inferred_state_component.h"
#include "game/components/guid_component.h"
#include "game/components/child_component.h"
#include "game/components/crosshair_component.h"
#include "augs/templates/container_templates.h"
#include "augs/templates/type_matching_and_indexing.h"

#include "generated/introspectors.h"

template <class D>
void relations_mixin<false, D>::make_as_child_of(const entity_id parent_id) const {
	auto& self = *static_cast<const D*>(this);

	auto& ch = self += components::child();
	ch.parent = parent_id;
}

template <class D>
void relations_mixin<false, D>::make_cloned_child_entities_recursive(const entity_id from_id) const {
	auto& self = *static_cast<const D*>(this);
	auto& cosm = self.get_cosmos();

	const const_entity_handle from = cosm[from_id];

	for_each_component_type([&](auto c) {
		using component_type = decltype(c);
		
		if (self.has<component_type>()) {
			auto& cloned_to_component = self.allocator::template get<component_type>();
			const auto& cloned_from_component = from.allocator::template get<component_type>();

			if constexpr(allows_nontriviality_v<component_type>) {
				component_type::clone_children(
					cosm,
					cloned_to_component, 
					cloned_from_component
				);
			}
			else {
				augs::introspect(
					augs::recursive([&](auto&& self, auto, auto& into, const auto& from) {
						if constexpr(std::is_same_v<decltype(into), child_entity_id&>) {
							into = cosm.clone_entity(from);
						}
						else {
							augs::introspect_if_not_leaf(augs::recursive(self), into, from);
						}
					}),
					cloned_to_component,
					cloned_from_component
				);
			}
		}
	});
}

template <class D>
void relations_mixin<false, D>::map_child_entity(
	const child_entity_name n, 
	const entity_id p
) const {
	get_id(n) = p;
}

template <class D>
void relations_mixin<false, D>::set_owner_body(const entity_id parent_id) const {
	auto& self = *static_cast<const D*>(this);
	self.get<components::fixtures>().set_owner_body(parent_id);
}

template <bool C, class D>
maybe_const_ref_t<C, child_entity_id> typename basic_relations_mixin<C, D>::get_id(const child_entity_name n) const {
	const auto& self = *static_cast<const D*>(this);

	switch (n) {
	case child_entity_name::CROSSHAIR_RECOIL_BODY:
		return self.get<components::crosshair>().recoil_entity;

	case child_entity_name::CHARACTER_CROSSHAIR:
		return self.get<components::sentience>().character_crosshair;

	case child_entity_name::CATRIDGE_BULLET:
		return self.get<components::catridge>().round;

	case child_entity_name::CATRIDGE_SHELL:
		return self.get<components::catridge>().shell;

	default:
		LOG("Random access abstraction for this child_entity_name is not implemented!");
		ensure(false);
		return get_id(child_entity_name::INVALID);
	}
}

template <bool C, class D>
typename basic_relations_mixin<C, D>::inventory_slot_handle_type basic_relations_mixin<C, D>::operator[](const slot_function func) const {
	auto& self = *static_cast<const D*>(this);
	return inventory_slot_handle_type(self.owner, inventory_slot_id(func, self.raw_id));
}

template <bool C, class D>
D basic_relations_mixin<C, D>::operator[](const child_entity_name child) const {
	auto& self = *static_cast<const D*>(this);
	return self.get_cosmos()[get_id(child)];
}

template <bool C, class D>
D basic_relations_mixin<C, D>::get_owner_body() const {
	auto& self = *static_cast<const D*>(this);
	const auto fixtures = self.find<components::fixtures>();
	return fixtures != nullptr ? self.get_cosmos()[fixtures.get_owner_body()] : self.get_cosmos()[entity_id()];
}

#if COSMOS_TRACKS_GUIDS
template <bool C, class D>
entity_guid basic_relations_mixin<C, D>::get_guid() const {
	auto& self = *static_cast<const D*>(this);
	return self.get<components::guid>().get_value();
}
#endif

template <bool C, class D>
D basic_relations_mixin<C, D>::get_parent() const {
	auto& self = *static_cast<const D*>(this);

	if (self.has<components::child>()) {
		return self.get_cosmos()[self.get<components::child>().parent];
	}
	else {
		return self.get_cosmos()[entity_id()];
	}
}

template class basic_relations_mixin<false, basic_entity_handle<false>>;
template class basic_relations_mixin<true, basic_entity_handle<true>>;
template class relations_mixin<false, basic_entity_handle<false>>;
template class relations_mixin<true, basic_entity_handle<true>>;
