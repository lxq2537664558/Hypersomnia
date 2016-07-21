#pragma once
#include <unordered_map>
#include <vector>
#include "game/entity_id.h"

#include "game/enums/sub_entity_name.h"
#include "game/enums/associated_entity_name.h"

struct entity_relations {
	std::unordered_map<sub_entity_name, entity_id> sub_entities_by_name;
	std::unordered_map<associated_entity_name, entity_id> associated_entities_by_name;

	std::vector<entity_id> sub_entities;

	entity_id parent;

	sub_entity_name name_as_sub_entity = sub_entity_name::INVALID;

	std::vector<entity_id> fixture_entities;
	entity_id owner_body;
};