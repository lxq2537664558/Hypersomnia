#pragma once
#include "augs/math/si_scaling.h"
#include "augs/templates/type_in_list_id.h"

#include "game/global_state/visibility_settings.h"
#include "game/global_state/pathfinding_settings.h"
#include "game/global_state/global_assets.h"
#include "game/global_state/entity_name_metas.h"

#include "game/detail/spells/all_spells.h"

using meter_tuple = put_all_meters_into_t<std::tuple>;
using spell_tuple = put_all_spells_into_t<std::tuple>;
using perk_tuple = put_all_perks_into_t<std::tuple>;

using spell_meta_id = type_in_list_id<spell_tuple>;
using perk_meta_id = type_in_list_id<meter_tuple>;

struct cosmos_global_state {
	// GEN INTROSPECTOR struct cosmos_global_state
	visibility_settings visibility;
	pathfinding_settings pathfinding;
	si_scaling si;

	entity_name_metas name_metas;
	global_assets assets;

	meter_tuple meters;
	spell_tuple spells;
	perk_tuple perks;
	// END GEN INTROSPECTOR
};

namespace std {
	template <class T>
	auto& get(const cosmos_global_state& s) {
		if constexpr(is_one_of_list_v<T, decltype(s.meters)>) {
			return std::get<T>(s.meters);
		}
		else if constexpr(is_one_of_list_v<T, decltype(s.perks)>) {
			return std::get<T>(s.perks);
		}
		else {
			static_assert(always_false_v<T>);
		}
	}

	template <class T>
	const auto& get(const cosmos_global_state& s) {
		if constexpr(is_one_of_list_v<T, decltype(s.meters)>) {
			return std::get<T>(s.meters);
		}
		else if constexpr(is_one_of_list_v<T, decltype(s.perks)>) {
			return std::get<T>(s.perks);
		}
		else {
			static_assert(always_false_v<T>);
		}
	}
}