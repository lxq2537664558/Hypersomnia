#include "particles_existence_component.h"
#include "game/detail/state_for_drawing_camera.h"

using namespace augs;

namespace components {
	void particles_existence::draw(const drawing_input& group_input) const {
		//for (auto& s : stream_slots)
		//	for (auto it : s.particles) {
		//		auto temp_alpha = it.face.color.a;
		//
		//		if (it.should_disappear) {
		//			auto alivity_multiplier = (it.max_lifetime_ms - it.lifetime_ms) / it.max_lifetime_ms;
		//
		//			auto desired_alpha = static_cast<rgba_channel>(alivity_multiplier * static_cast<float>(temp_alpha));
		//			if (it.alpha_levels > 0) {
		//				it.face.color.a = desired_alpha == 0 ? 0 : ((255 / it.alpha_levels) * (1 + (desired_alpha / (255 / it.alpha_levels))));
		//			}
		//			else {
		//				it.face.color.a = desired_alpha;
		//			}
		//
		//			// it.face.size_multiplier.set(alivity_multiplier, alivity_multiplier);
		//		}
		//
		//		components::sprite::drawing_input in(group_input.target_buffer);
		//
		//		in.renderable_transform = it.ignore_rotation ? components::transform(it.pos, 0) : components::transform({ it.pos, it.rotation });
		//		in.camera_transform = group_input.camera_transform;
		//		in.visible_world_area = group_input.visible_world_area;
		//		//in.renderable_transform += in.renderable_transform;
		//		it.face.draw(in);
		//		//it.face.color.a = temp_alpha;
		//		// it.face.size_multiplier.set(1, 1);
		//	}
	}

}