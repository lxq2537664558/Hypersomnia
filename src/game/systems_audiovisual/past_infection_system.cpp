#include "past_infection_system.h"
#include "game/transcendental/entity_handle.h"
#include "game/transcendental/cosmos.h"
#include "game/components/flags_component.h"
#include "augs/templates/container_templates.h"

void past_infection_system::infect(const const_entity_handle id) {
	if (!is_infected(id)) {
		infected_entities.insert(id);
	}
}

bool past_infection_system::is_infected(const const_entity_handle id) const {
	return found_in(infected_entities, id) || id.get_flag(entity_flag::IS_PAST_CONTAGIOUS);
}

void past_infection_system::uninfect(const entity_id id) {
	infected_entities.erase(id);
}