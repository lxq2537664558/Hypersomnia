#include "game/detail/ai/behaviours.h"
#include "game/transcendental/entity_id.h"

#include "game/components/attitude_component.h"
#include "game/components/movement_component.h"
#include "game/components/crosshair_component.h"
#include "game/detail/entity_scripts.h"

#include "game/transcendental/cosmos.h"
#include "game/transcendental/logic_step.h"

namespace behaviours {
	tree::goal_availability minimize_recoil_through_movement::goal_resolution(tree::state_of_traversal& t) const {
		const auto subject = t.subject;
		const auto& cosmos = t.step.cosm;
		const auto crosshair = subject[child_entity_name::CHARACTER_CROSSHAIR];
		const auto& attitude = subject.get<components::attitude>();
		const auto currently_attacked_visible_entity = cosmos[attitude.currently_attacked_visible_entity];

		if (currently_attacked_visible_entity.alive() && crosshair.alive()) {
			auto recoil = crosshair[child_entity_name::CROSSHAIR_RECOIL_BODY];
			auto& c = crosshair.get<components::crosshair>();

			minimize_recoil_through_movement_goal goal;

			const auto subject_orientation = subject.get_logic_transform().get_orientation();

			goal.movement_direction = (c.base_offset - subject_orientation * c.base_offset.length());
			t.set_goal(goal);
			return tree::goal_availability::SHOULD_EXECUTE;
		}

		return tree::goal_availability::ALREADY_ACHIEVED;
	}

	void minimize_recoil_through_movement::execute_leaf_goal_callback(tree::execution_occurence o, tree::state_of_traversal& t) const {
		const auto subject = t.subject;
		auto& movement = subject.get<components::movement>();

		if (o == tree::execution_occurence::LAST) {
			movement.reset_movement_flags();
		}
		else {
			movement.set_flags_from_closest_direction(t.get_goal<minimize_recoil_through_movement_goal>().movement_direction);
		}
	}
}