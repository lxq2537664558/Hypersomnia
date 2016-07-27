#include "driver_system.h"
#include "game/messages/trigger_hit_confirmation_message.h"
#include "game/messages/collision_message.h"
#include "game/messages/intent_message.h"

#include "game/components/trigger_component.h"
#include "game/components/car_component.h"
#include "game/components/input_receiver_component.h"
#include "game/components/movement_component.h"
#include "game/components/rotation_copying_component.h"
#include "game/components/physics_component.h"
#include "game/components/force_joint_component.h"

#include "game/transcendental/cosmos.h"
#include "game/temporary_systems/physics_system.h"

#include "game/components/driver_component.h"
#include "game/components/physics_component.h"
#include "game/components/transform_component.h"

#include "game/detail/physics_scripts.h"

#include "game/transcendental/entity_handle.h"
#include "game/transcendental/step.h"

void driver_system::assign_drivers_from_successful_trigger_hits(fixed_step& step) {
	auto& cosmos = step.cosm;
	auto& delta = step.get_delta();
	auto& confirmations = step.messages.get_queue<messages::trigger_hit_confirmation_message>();

	for (auto& e : confirmations) {
		auto subject_car = cosmos[cosmos[e.trigger].get<components::trigger>().entity_to_be_notified];

		if (subject_car.dead())
			continue;

		auto* maybe_car = cosmos[subject_car].find<components::car>();

		if (maybe_car && e.trigger == maybe_car->left_wheel_trigger)
			assign_car_ownership(cosmos[e.detector_body], subject_car);
	}
}

void driver_system::release_drivers_due_to_ending_contact_with_wheel(fixed_step& step) {
	auto& cosmos = step.cosm;
	auto& delta = step.get_delta();
	auto& contacts = step.messages.get_queue<messages::collision_message>();
	auto& physics = cosmos.temporary_systems.get<physics_system>();

	for (auto& c : contacts) {
		if (c.type == messages::collision_message::event_type::END_CONTACT) {
			auto driver = cosmos[c.subject];
			auto car = cosmos[c.collider].get_owner_body();

			auto* maybe_driver = driver.find<components::driver>();

			if (maybe_driver) {
				if (maybe_driver->owned_vehicle == car) {
					release_car_ownership(driver);
					driver.get<components::movement>().make_inert_for_ms = 500.f;
				}
			}
		}
	}
}
void driver_system::release_drivers_due_to_requests(fixed_step& step) {
	auto& cosmos = step.cosm;
	auto& delta = step.get_delta();
	auto& intents = step.messages.get_queue<messages::intent_message>();

	for (auto& e : intents)
		if (e.intent == intent_type::RELEASE_CAR && e.pressed_flag)
			release_car_ownership(cosmos[e.subject]);
}

bool driver_system::release_car_ownership(entity_handle driver) {
	return change_car_ownership(driver, driver.get_cosmos().get_handle(entity_id()), true);
}

bool driver_system::assign_car_ownership(entity_handle driver, entity_handle car) {
	return change_car_ownership(driver, car, false);
}

bool driver_system::change_car_ownership(entity_handle driver_entity, entity_handle car_entity, bool lost_ownership) {
	auto& driver = driver_entity.get<components::driver>();
	auto& cosmos = driver_entity.get_cosmos();
	auto& physics = cosmos.temporary_systems.get<physics_system>();

	auto* maybe_rotation_copying = driver_entity.find<components::rotation_copying>();
	bool has_physics = driver_entity.has<components::physics>();
	auto* maybe_movement = driver_entity.find<components::movement>();
	auto& force_joint = driver_entity.get<components::force_joint>();

	if (!lost_ownership) {
		auto& car = car_entity.get<components::car>();

		if (cosmos[car.current_driver].alive())
			return false;

		driver.owned_vehicle = car_entity;
		car.current_driver = driver_entity;
		force_joint.chased_entity = car.left_wheel_trigger;
		driver_entity.get<components::processing>().enable_in(processing_subjects::WITH_FORCE_JOINT);

		if (maybe_movement) {
			maybe_movement->reset_movement_flags();
			maybe_movement->enable_braking_damping = false;
			maybe_movement->enable_animation = false;
		}

		if (maybe_rotation_copying && has_physics) {
			maybe_rotation_copying->update_value = false;
		}

		if (has_physics) {
			auto& physics = driver_entity.get<components::physics>();
			physics.set_transform(car.left_wheel_trigger);
			physics.set_velocity(vec2(0, 0));
			resolve_density_of_associated_fixtures(driver_entity);
		}
	}
	else {
		auto& car = cosmos[driver.owned_vehicle].get<components::car>();

		driver.owned_vehicle.unset();
		car.current_driver.unset();
		driver_entity.get<components::processing>().disable_in(processing_subjects::WITH_FORCE_JOINT);

		if (maybe_movement) {
			maybe_movement->reset_movement_flags();
			maybe_movement->enable_braking_damping = true;
			maybe_movement->enable_animation = true;

			maybe_movement->moving_left = car.turning_left;
			maybe_movement->moving_right = car.turning_right;
			maybe_movement->moving_forward = car.accelerating;
			maybe_movement->moving_backward = car.deccelerating;
		}
		
		car.reset_movement_flags();

		if (maybe_rotation_copying && has_physics) {
			maybe_rotation_copying->update_value = true;
		}

		if (has_physics) {
			resolve_density_of_associated_fixtures(driver_entity);
		}
	}

	// networking the message, since it was successful

	return true;
}