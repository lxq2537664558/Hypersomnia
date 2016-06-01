#include "behaviours.h"
#include "game/components/attitude_component.h"
#include "game/components/visibility_component.h"
#include "game/components/movement_component.h"
#include "game/components/sentience_component.h"
#include "game/components/crosshair_component.h"
#include "game/components/gun_component.h"
#include "entity_system/entity.h"

#include "game/detail/entity_scripts.h"
#include "game/detail/inventory_utils.h"
#include "game/detail/inventory_slot.h"

namespace behaviours {
	tree::goal_availability immediate_evasion::goal_resolution(tree::state_of_traversal& t) const {
		auto subject = t.instance.user_input;
		auto& visibility = subject->get<components::visibility>();
		auto& los = visibility.line_of_sight_layers[components::visibility::LINE_OF_SIGHT];

		immediate_evasion_goal goal;

		float total_danger = 0.f;

		for (auto& s : los.visible_dangers) {
			auto danger = assess_danger(subject, s);
			ensure(danger.amount > 0);

			total_danger += danger.amount;
			goal.dangers.push_back(danger);
		}

		if (total_danger < subject->get<components::sentience>().minimum_danger_amount_to_evade) {
			return tree::goal_availability::ALREADY_ACHIEVED;
		}
		else {
			t.set_goal(goal);
			return tree::goal_availability::SHOULD_EXECUTE;
		}
	}

	void immediate_evasion::execute_leaf_goal_callback(tree::execution_occurence o, tree::state_of_traversal& t) const {
		auto subject = t.instance.user_input;
		auto& movement = subject->get<components::movement>();

		if (o == tree::execution_occurence::LAST) {
			movement.reset_movement_flags();
			return;
		}

		auto& goal = t.get_goal<immediate_evasion_goal>();

		vec2 resultant_evasion;

		for (size_t i = 0; i < goal.dangers.size(); ++i) {
			resultant_evasion += goal.dangers[i].recommended_evasion * goal.dangers[i].amount;
		}

		movement.set_flags_from_target_direction(resultant_evasion);
	}


	tree::goal_availability target_closest_enemy::goal_resolution(tree::state_of_traversal& t) const {
		return tree::goal_availability::SHOULD_EXECUTE;
	}

	void target_closest_enemy::execute_leaf_goal_callback(tree::execution_occurence occ, tree::state_of_traversal& t) const {
		if (occ == tree::execution_occurence::LAST)
			return;
		
		auto subject = t.instance.user_input;
		auto pos = subject->get<components::transform>().pos;
		auto& visibility = subject->get<components::visibility>();
		auto& los = visibility.line_of_sight_layers[components::visibility::LINE_OF_SIGHT];

		augs::entity_id closest_hostile;

		float min_distance = std::numeric_limits<float>::max();

		for (auto& s : los.visible_sentiences) {
			auto att = calculate_attitude(s, subject);

			if (att == attitude_type::WANTS_TO_KILL || att == attitude_type::WANTS_TO_KNOCK_UNCONSCIOUS) {
				auto dist = (s->get<components::transform>().pos - pos).length_sq();
				
				if (dist < min_distance) {
					closest_hostile = s;
					min_distance = dist;
				}
			}
		}

		subject->get<components::attitude>().chosen_target = closest_hostile;

		if (subject.has(sub_entity_name::CHARACTER_CROSSHAIR)) {
			auto crosshair = subject[sub_entity_name::CHARACTER_CROSSHAIR];
			auto& crosshair_offset = crosshair->get<components::crosshair>().base_offset;
			
			if (closest_hostile.alive()) {
				crosshair_offset = closest_hostile->get<components::transform>().pos - pos;
			}
			else {
				auto owner_body = components::physics::get_owner_body_entity(subject);
				
				if(owner_body.alive())
					crosshair_offset = owner_body->get<components::physics>().velocity();
			}
		}
	}

	tree::goal_availability pull_trigger::goal_resolution(tree::state_of_traversal& t) const {
		auto subject = t.instance.user_input;
		auto chosen_target = subject->get<components::attitude>().chosen_target;

		if (chosen_target.alive() && guns_wielded(subject).size() > 0) {
			return tree::goal_availability::SHOULD_EXECUTE;
		}
		
		return tree::goal_availability::CANT_EXECUTE;
	}
	
	void pull_trigger::execute_leaf_goal_callback(tree::execution_occurence o, tree::state_of_traversal& t) const {
		auto subject = t.instance.user_input;
		auto wielded = guns_wielded(subject);

		for (auto& w : wielded) {
			if(o == tree::execution_occurence::LAST)
				w->get<components::gun>().trigger_pressed = false;
			else
				w->get<components::gun>().trigger_pressed = true;
		}
	}
}