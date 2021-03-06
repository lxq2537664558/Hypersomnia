#pragma once
#include "augs/build_settings/setting_entity_tracks_name_for_debug.h"
#include "augs/misc/pooled_object_id.h"
#include "augs/misc/trivially_copyable_tuple.h"
#include "augs/templates/type_matching_and_indexing.h"
#include "augs/templates/transform_types.h"
#include "augs/templates/component_traits.h"
#include "augs/pad_bytes.h"

namespace augs {
	template <class... components>
	class component_aggregate {
	public:
		using dynamic_components_list = filter_types<apply_negation_t<is_component_fundamental>, components...>;
		using fundamental_components_list = filter_types<is_component_fundamental, components...>;

		using dynamic_component_id_tuple = 
			replace_list_type_t<
				transform_types_in_list_t<
					dynamic_components_list, 
					make_pooled_object_id
				>, 
				trivially_copyable_tuple
			>
		;

		using fundamental_components_tuple = 
			replace_list_type_t<
				fundamental_components_list, 
				trivially_copyable_tuple
			>
		;

		// GEN INTROSPECTOR class augs::component_aggregate class... components
		fundamental_components_tuple fundamentals;
		dynamic_component_id_tuple component_ids;
#if ENTITY_TRACKS_NAME_FOR_DEBUG
		const std::wstring* debug_name = nullptr;
#else
		pad_bytes<sizeof(const std::wstring*)> for_release_debug_compatibility;
#endif
		// END GEN INTROSPECTOR

		template <class component>
		void set_id(const pooled_object_id<component> to) {
			std::get<pooled_object_id<component>>(component_ids) = to;
		}

		template <class component>
		auto get_id() const {
			return std::get<pooled_object_id<component>>(component_ids);
		}
	};
}
