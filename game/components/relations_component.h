#pragma once
#include <unordered_map>
#include <vector>
#include "game/entity_id.h"
#include "game/types_specification/full_entity_definition_declaration.h"

namespace components {
	struct relations {
		std::unordered_map<sub_definition_name, full_entity_definition> sub_definitions_by_name;
		std::unordered_map<sub_entity_name, entity_id> sub_entities_by_name;
		std::unordered_map<associated_entity_name, entity_id> associated_entities_by_name;

		std::vector<entity_id> sub_entities;

		entity_id parent;
		entity_id self_id;

		sub_entity_name name_as_sub_entity = sub_entity_name::INVALID;
	};
}