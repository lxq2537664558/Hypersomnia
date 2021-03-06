#include "force_joint_system.h"
#include "game/transcendental/entity_id.h"
#include "augs/log.h"

#include "game/components/force_joint_component.h"
#include "game/components/rigid_body_component.h"
#include "game/components/special_physics_component.h"
#include "game/components/transform_component.h"
#include "game/components/rotation_copying_component.h"

#include "game/transcendental/cosmos.h"

#include "game/transcendental/entity_handle.h"
#include "game/transcendental/logic_step.h"

void force_joint_system::apply_forces_towards_target_entities(const logic_step step) {
	auto& cosmos = step.cosm;
	const auto delta = step.get_delta();

	cosmos.for_each(
		processing_subjects::WITH_FORCE_JOINT,
		[&](const auto it) {
			if (!it.has<components::rigid_body>()) {
				return;
			}

			const auto& rigid_body = it.get<components::rigid_body>();

			if (!rigid_body.is_constructed()) {
				return;
			}

			const auto& force_joint = it.get<components::force_joint>();
			const auto chased_entity = cosmos[force_joint.chased_entity];

			if (chased_entity.dead()) {
				return;
			}

			const auto chased_entity_transform = chased_entity.get_logic_transform();
			const auto chased_transform = chased_entity_transform * force_joint.chased_entity_offset;

			auto direction = chased_transform.pos - rigid_body.get_position();
			const auto distance = direction.length();
			direction.normalize_hint(distance);

			if (force_joint.divide_transform_mode) {
				const auto current_transform = it.get_logic_transform();
				const auto interpolated = augs::interp(current_transform, chased_transform, 1.0 - 1.0 / (1.0 + delta.in_seconds() * (60.0)));
				//LOG("Cur: %x,%x, Chas: %x,%x, Inter: %x,%x", current_transform.pos, current_transform.rotation, chased_entity_transform.pos, chased_entity_transform.rotation, interpolated.pos, interpolated.rotation);
				rigid_body.set_transform(interpolated);
			}
			else {
				float force_length = force_joint.force_towards_chased_entity;

				if (distance < force_joint.distance_when_force_easing_starts) {
					const auto mult = distance / force_joint.distance_when_force_easing_starts;
					force_length *= pow(mult, force_joint.power_of_force_easing_multiplier);
				}

				const auto force_for_chaser = direction * force_length * (1.f - force_joint.percent_applied_to_chased_entity);
				const auto force_for_chased = -force_for_chaser * force_joint.percent_applied_to_chased_entity;

				const bool is_force_epsilon = force_for_chaser.length() < 500;

				const auto& offsets = force_joint.force_offsets;

				const auto offsets_count = static_cast<int>(offsets.size());

				//if (!is_force_epsilon) 
				{
					for (const auto offset : offsets) {
						rigid_body.apply_force(force_for_chaser * rigid_body.get_mass() / offsets_count, offset);
					}

					//LOG("F: %x, %x, %x", force_for_chaser, rigid_body.velocity(), AS_INTV rigid_body.get_position());
				}
				//else if (is_force_epsilon && rigid_body.velocity().is_epsilon(1.f)) {
				//	rigid_body.set_velocity(vec2(0, 0));
				//	//rigid_body.set_transform(components::transform(chased_transform.pos, rigid_body.get_angle()));
				//	LOG("Zeroed");
				//}

				if (force_for_chased.length() > 5) {
					const auto& chased_physics = cosmos[force_joint.chased_entity].get<components::rigid_body>();
					chased_physics.apply_force(force_for_chaser * chased_physics.get_mass());
				}

				//if (force_joint.consider_rotation)
				//	it.get<components::rotation_copying>().target_angle = chased_transform.rotation;

				//LOG("F: %x", rigid_body.body->GetLinearDamping());
			}
		}
	);
}