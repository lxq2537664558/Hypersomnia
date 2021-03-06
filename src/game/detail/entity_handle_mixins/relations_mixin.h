#pragma once
#include <vector>
#include "game/detail/inventory/inventory_slot_handle_declaration.h"
#include "game/transcendental/entity_handle_declaration.h"

#include "game/transcendental/entity_id.h"

#include "game/enums/slot_function.h"
#include "game/enums/child_entity_name.h"

#include "game/build_settings.h"
#include "augs/build_settings/setting_empty_bases.h"

#include "augs/templates/maybe_const.h"
#include "augs/templates/template_logic.h"

struct entity_relations;

template<bool is_const, class entity_handle_type>
class basic_relations_mixin {
protected:
	typedef basic_inventory_slot_handle<is_const> inventory_slot_handle_type;
public:
	entity_handle_type get_parent() const;
	
	entity_handle_type get_owner_body() const;
	
	auto get_fixture_entities() const {
		const auto self = *static_cast<const entity_handle_type*>(this);

		return self.get<components::rigid_body>().get_fixture_entities();
	}

	auto get_attached_joints() const {
		const auto self = *static_cast<const entity_handle_type*>(this);

		return self.get<components::rigid_body>().get_attached_joints();
	}

#if COSMOS_TRACKS_GUIDS
	entity_guid get_guid() const;
#endif

	maybe_const_ref_t<is_const, child_entity_id> get_id(const child_entity_name) const;

	inventory_slot_handle_type operator[](const slot_function) const;
	entity_handle_type operator[](const child_entity_name) const;
	
	template <class F>
	void for_each_child_entity_recursive(F&& callback) const {
		const auto self = *static_cast<const entity_handle_type*>(this);
		auto& cosmos = self.get_cosmos();

		self.for_each_component(
			[&cosmos, &callback](auto& subject_component) {
				augs::introspect(
					[&](auto, auto& member) {
						if constexpr(std::is_same_v<std::decay_t<decltype(member)>, child_entity_id>) {
							const auto child_handle = cosmos[member];

							if (child_handle.alive() && callback(child_handle)) {
								child_handle.for_each_child_entity_recursive(std::forward<F>(callback));
							}
						}
					},
					subject_component
				);
			}
		);
	}
};

template<bool, class>
class relations_mixin;

template<class entity_handle_type>
class EMPTY_BASES relations_mixin<false, entity_handle_type> : public basic_relations_mixin<false, entity_handle_type> {
protected:
public:
	void make_as_child_of(const entity_id) const;

	void set_owner_body(const entity_id) const;
	void make_cloned_child_entities_recursive(const entity_id copied) const;

	void map_child_entity(const child_entity_name n, const entity_id p) const;
};

template<class entity_handle_type>
class EMPTY_BASES relations_mixin<true, entity_handle_type> : public basic_relations_mixin<true, entity_handle_type> {
public:
};
