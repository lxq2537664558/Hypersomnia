#include "processing_lists_system.h"
#include "game/components/force_joint_component.h"
#include "game/components/gui_element_component.h"
#include "game/components/input_receiver_component.h"
#include "game/components/trigger_query_detector_component.h"
#include "game/cosmos.h"
#include "game/entity_handle.h"

void processing_lists_system::destruct(const_entity_handle handle) {
	auto id = handle.get_id();
	size_t index = id.indirection_index;

	if (per_entity_cache[index].is_constructed) {
		for (auto& list : lists)
			remove_element(list.second, id);

		per_entity_cache[index] = cache();
	}
}

void processing_lists_system::construct(const_entity_handle handle) {
	if (!handle.has<components::processing>()) return;

	auto id = handle.get_id();
	size_t index = id.indirection_index;
	
	ensure(!per_entity_cache[index].is_constructed);

	auto& processing = handle.get<components::processing>();
	
	if (processing.is_activated()) {
		for (auto& list : lists)
			if (processing.is_in(list.first))
				list.second.push_back(id);

		per_entity_cache[index].is_constructed = true;
	}
}

void processing_lists_system::reserve_caches_for_entities(size_t n) {
	per_entity_cache.reserve(n);
}

std::vector<entity_handle> processing_lists_system::get(processing_subjects list, cosmos& cosmos) const {
	return cosmos[lists.at(list)];
}

std::vector<const_entity_handle> processing_lists_system::get(processing_subjects list, const cosmos& cosmos) const {
	return cosmos[lists.at(list)];
}