#include "testbed.h"
#include "game/ingredients/ingredients.h"

#include "game/cosmos.h"
#include "window_framework/window.h"

#include "game/resources/manager.h"
#include "game/assets/texture_id.h"
#include "game/assets/atlas_id.h"

#include "game/systems/input_system.h"
#include "game/systems/render_system.h"
#include "game/stateful_systems/gui_system.h"
#include "game/systems/visibility_system.h"
#include "game/systems/pathfinding_system.h"
#include "game/components/position_copying_component.h"
#include "game/components/sentience_component.h"
#include "game/components/item_component.h"
#include "game/components/name_component.h"
#include "game/components/attitude_component.h"
#include "game/components/camera_component.h"
#include "game/components/pathfinding_component.h"
#include "game/enums/party_category.h"

#include "game/messages/crosshair_intent_message.h"
#include "game/detail/item_slot_transfer_request.h"

#include "game/detail/inventory_slot.h"
#include "game/detail/inventory_utils.h"

#include "augs/filesystem/file.h"
#include "misc/time.h"

#include "rendering_scripts/all.h"
#include "resource_setups/all.h"

#include "texture_baker/font.h"

#include "game/detail/inventory_utils.h"
#include "game/systems/item_system.h"

#include "gui/text/printer.h"
#include "log.h"
using namespace augs;

namespace scene_managers {
	void testbed::populate_world_with_entities(fixed_step& step) {
		auto& world = step.cosm;
		auto& window = *window::glwindow::get_current();
		auto window_rect = window.get_screen_rect();

		world.stateful_systems.get<gui_system>().resize(vec2i(window_rect.w, window_rect.h));

		auto crate = prefabs::create_crate(world, vec2(200, 200 + 300), vec2i(100, 100) / 3);
		auto crate2 = prefabs::create_crate(world, vec2(400, 200 + 400), vec2i(300, 300));
		auto crate4 = prefabs::create_crate(world, vec2(500, 200 + 0), vec2i(100, 100));

		for (int x = -4; x < 4; ++x) {
			for (int y = -4; y < 4; ++y) {
				auto obstacle = prefabs::create_crate(world, vec2(2000 + x * 300, 2000 + y * 300), vec2i(100, 100));
			}
		}

		for (int x = -4 * 1; x < 4 * 1; ++x)
		{
			auto frog = world.create_entity("frog");
			ingredients::sprite(frog, vec2(100 + x * 40, 200 + 400), assets::texture_id::TEST_SPRITE, augs::white, render_layer::DYNAMIC_BODY);
			ingredients::see_through_dynamic_body(frog);
		}

		auto car = prefabs::create_car(world, components::transform(-300, -600, -90));
		auto car2 = prefabs::create_car(world, components::transform(-800, -600, -90));
		auto car3 = prefabs::create_car(world, components::transform(-1300, -600, -90));

		auto motorcycle = prefabs::create_motorcycle(world, components::transform(0, -600, -90));
		prefabs::create_motorcycle(world, components::transform(100, -600, -90));

		auto camera = world.create_entity("camera");
		ingredients::camera(camera, window_rect.w, window_rect.h);
		world_camera = camera;

		auto bg_size = assets::get_size(assets::texture_id::TEST_BACKGROUND);

		for (int x = -4*10; x < 4 * 10; ++x)
			for (int y = -4 * 10; y < 4 * 10; ++y)
			{
				auto background = world.create_entity("bg[-]");
				ingredients::sprite(background, vec2(-1000, 0) + vec2(x, y) * (bg_size + vec2(1500, 550)), assets::texture_id::TEST_BACKGROUND, augs::white, render_layer::GROUND);
				//ingredients::standard_static_body(background);

				auto street = world.create_entity("street[-]");
				ingredients::sprite_scalled(street, vec2(-1000, 0) + vec2(x, y) * (bg_size + vec2(1500, 700)) - vec2(1500, 700),
					vec2(3000, 3000),
					assets::texture_id::TEST_BACKGROUND, augs::gray1, render_layer::UNDER_GROUND);
			}

		const int num_characters = 2;

		std::vector<entity_handle> new_characters;

		for (int i = 0; i < num_characters; ++i) {
			auto new_character = prefabs::create_character(world, vec2(i * 300, 0));
			new_character.set_debug_name(typesafe_sprintf("player%x", i));
			
			new_characters.push_back(new_character);

			if (i == 0) {
				new_character.get<components::sentience>().health.value = 800;
				new_character.get<components::sentience>().health.maximum = 800;
			}
			if (i == 1) {
				new_character.get<components::transform>().pos.set(2800, 700);
				new_character.get<components::attitude>().parties = party_category::RESISTANCE_CITIZEN;
				new_character.get<components::attitude>().hostile_parties = party_category::METROPOLIS_CITIZEN;
				new_character.get<components::attitude>().maximum_divergence_angle_before_shooting = 25;
				new_character.get<components::sentience>().minimum_danger_amount_to_evade = 20;
				new_character.get<components::sentience>().health.value = 300;
				new_character.get<components::sentience>().health.maximum = 300;
				ingredients::standard_pathfinding_capability(new_character);
				ingredients::soldier_intelligence(new_character);
			}
			if (i == 2) {
				new_character.get<components::sentience>().health.value = 38;
			}
			if (i == 5) {
				new_character.get<components::attitude>().parties = party_category::METROPOLIS_CITIZEN;
				new_character.get<components::attitude>().hostile_parties = party_category::RESISTANCE_CITIZEN;
				new_character.get<components::attitude>().maximum_divergence_angle_before_shooting = 25;
				new_character.get<components::sentience>().minimum_danger_amount_to_evade = 20;
				new_character.get<components::sentience>().health.value = 300;
				new_character.get<components::sentience>().health.maximum = 300;
				ingredients::standard_pathfinding_capability(new_character);
				ingredients::soldier_intelligence(new_character);
			}
		}

		name_entity(world[new_characters[0]], entity_name::PERSON, L"Attacker");

		ingredients::inject_window_input_to_character(world[new_characters[current_character]], camera);

		prefabs::create_sample_suppressor(world, vec2(300, -500));

		bool many_charges = false;

		auto rifle = prefabs::create_sample_rifle(world, vec2(100, -500), 
			prefabs::create_sample_magazine(world, vec2(100, -650), many_charges ? "10" : "0.3",
				prefabs::create_cyan_charge(world, vec2(0, 0), many_charges ? 1000 : 30)));

		auto rifle2 = prefabs::create_sample_rifle(world, vec2(100, -500 + 50), 
			prefabs::create_sample_magazine(world, vec2(100, -650), true ? "10" : "0.3",
				prefabs::create_cyan_charge(world, vec2(0, 0), true ? 1000 : 30)));

		prefabs::create_sample_rifle(world, vec2(100, -500 + 100));

		prefabs::create_pistol(world, vec2(300, -500 + 50));
		
		auto pis2 = prefabs::create_pistol(world, vec2(300, 50),
			prefabs::create_sample_magazine(world, vec2(100, -650), "0.4",
				prefabs::create_green_charge(world, vec2(0, 0), 40)));
		
		auto submachine = prefabs::create_submachine(world, vec2(500, -500 + 50), 
			prefabs::create_sample_magazine(world, vec2(100 - 50, -650), many_charges ? "10" : "0.5", prefabs::create_pink_charge(world, vec2(0, 0), many_charges ? 500 : 50)));

		prefabs::create_submachine(world, vec2(0, -1000),
			prefabs::create_sample_magazine(world, vec2(100 - 50, -650), many_charges ? "10" : "0.5", prefabs::create_pink_charge(world, vec2(0, 0), many_charges ? 500 : 50)));

		prefabs::create_submachine(world, vec2(150, -1000 + 150),
			prefabs::create_sample_magazine(world, vec2(100 - 50, -650), many_charges ? "10" : "0.5", prefabs::create_pink_charge(world, vec2(0, 0), many_charges ? 500 : 50)));

		prefabs::create_submachine(world, vec2(300, -1000 + 300),
			prefabs::create_sample_magazine(world, vec2(100 - 50, -650), many_charges ? "10" : "0.5", prefabs::create_pink_charge(world, vec2(0, 0), many_charges ? 500 : 50)));

		prefabs::create_submachine(world, vec2(450, -1000 + 450),
			prefabs::create_sample_magazine(world, vec2(100 - 50, -650), many_charges ? "10" : "0.5", prefabs::create_pink_charge(world, vec2(0, 0), many_charges ? 500 : 50)));


		prefabs::create_sample_magazine(world, vec2(100 - 50, -650));
		prefabs::create_sample_magazine(world, vec2(100 - 100, -650), "0.30");
		//prefabs::create_pink_charge(world, vec2(100, 100));
		//prefabs::create_pink_charge(world, vec2(100, -400));
		//prefabs::create_pink_charge(world, vec2(150, -400));
		//prefabs::create_pink_charge(world, vec2(200, -400));
		prefabs::create_cyan_charge(world, vec2(150, -500));
		prefabs::create_cyan_charge(world, vec2(200, -500));

		prefabs::create_cyan_urban_machete(world, vec2(100, 100));
		auto second_machete = prefabs::create_cyan_urban_machete(world, vec2(0, 0));

		auto backpack = prefabs::create_sample_backpack(world, vec2(200, -650));
		prefabs::create_sample_backpack(world, vec2(200, -750));

		perform_transfer({ backpack, new_characters[0][slot_function::SHOULDER_SLOT] }, step);
		perform_transfer({ submachine, new_characters[0][slot_function::PRIMARY_HAND] }, step);
		perform_transfer({ rifle, new_characters[0][slot_function::SECONDARY_HAND] }, step);

		if (num_characters > 1) {
			name_entity(new_characters[1], entity_name::PERSON, L"Enemy");
			perform_transfer({ rifle2, new_characters[1][slot_function::PRIMARY_HAND] }, step);
		}

		if (num_characters > 2) {
			name_entity(new_characters[2], entity_name::PERSON, L"Swordsman");
			perform_transfer({ second_machete, new_characters[2][slot_function::PRIMARY_HAND] }, step);
		}

		if (num_characters > 3) {
			name_entity(new_characters[3], entity_name::PERSON, L"Medic");
			perform_transfer({ pis2, new_characters[3][slot_function::PRIMARY_HAND] }, step);
		}

		if (num_characters > 5) {
			auto new_item = prefabs::create_submachine(world, vec2(0, -1000),
				prefabs::create_sample_magazine(world, vec2(100 - 50, -650), true ? "10" : "0.5", prefabs::create_pink_charge(world, vec2(0, 0), true ? 500 : 50)));
			
			perform_transfer({ new_item, new_characters[5][slot_function::PRIMARY_HAND] }, step);
		}

		auto& active_context = world.settings.input;

		active_context.map_key_to_intent(window::event::keys::W, intent_type::MOVE_FORWARD);
		active_context.map_key_to_intent(window::event::keys::S, intent_type::MOVE_BACKWARD);
		active_context.map_key_to_intent(window::event::keys::A, intent_type::MOVE_LEFT);
		active_context.map_key_to_intent(window::event::keys::D, intent_type::MOVE_RIGHT);

		active_context.map_event_to_intent(window::event::mousemotion, intent_type::MOVE_CROSSHAIR);
		active_context.map_key_to_intent(window::event::keys::LMOUSE, intent_type::CROSSHAIR_PRIMARY_ACTION);
		active_context.map_key_to_intent(window::event::keys::RMOUSE, intent_type::CROSSHAIR_SECONDARY_ACTION);
		
		active_context.map_key_to_intent(window::event::keys::E, intent_type::USE_BUTTON);
		active_context.map_key_to_intent(window::event::keys::LSHIFT, intent_type::WALK);
		
		active_context.map_key_to_intent(window::event::keys::G, intent_type::THROW_PRIMARY_ITEM);
		active_context.map_key_to_intent(window::event::keys::H, intent_type::HOLSTER_PRIMARY_ITEM);
		
	    active_context.map_key_to_intent(window::event::keys::BACKSPACE, intent_type::SWITCH_LOOK);

		active_context.map_key_to_intent(window::event::keys::LCTRL, intent_type::START_PICKING_UP_ITEMS);
		active_context.map_key_to_intent(window::event::keys::CAPSLOCK, intent_type::SWITCH_CHARACTER);

		active_context.map_key_to_intent(window::event::keys::SPACE, intent_type::SPACE_BUTTON);
		active_context.map_key_to_intent(window::event::keys::MOUSE4, intent_type::SWITCH_TO_GUI);

		//draw_bodies.push_back(crate2);
		//draw_bodies.push_back(new_characters[0]);
		//draw_bodies.push_back(backpack);

		world.settings.visibility.epsilon_ray_distance_variation = 0.001;
		world.settings.visibility.epsilon_threshold_obstacle_hit = 10;
		world.settings.visibility.epsilon_distance_vertex_hit = 1;

		show_profile_details = true;

		world.settings.pathfinding.draw_memorised_walls = 1;
		world.settings.pathfinding.draw_undiscovered = 1;
		
		characters = to_id_vector(new_characters);
		// _controlfp(0, _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID | _EM_DENORMAL);
	}

	void testbed::post_solve(fixed_step& step) {
		for (auto& in : step.entropy.local) {
			if (in.key_event == window::event::PRESSED) {
				if (in.key == window::event::keys::DASH) {
					show_profile_details = !show_profile_details;
				}
			}
		}
		
		auto& cosmos = step.cosm;
		auto& delta = step.get_delta();
		auto inputs = step.messages.get_queue<messages::crosshair_intent_message>();

		for (auto& it : step.messages.get_queue<messages::unmapped_intent_message>()) {
			if (it.intent == intent_type::SWITCH_CHARACTER && it.pressed_flag) {
				++current_character;
				current_character %= characters.size();
				
				ingredients::inject_window_input_to_character(cosmos[characters[current_character]], cosmos[world_camera]);
			}
		}

		//for (auto& tested : draw_bodies) {
		//	auto& s = tested.get<components::physics_definition>();
		//
		//	auto& lines = renderer::get_current().logic_lines;
		//
		//	auto vv = s.fixtures[0].debug_original;
		//
		//	for (int i = 0; i < vv.size(); ++i) {
		//		auto& tt = tested.get<components::transform>();
		//		auto pos = tt.pos;
		//
		//		lines.draw_cyan((pos + vv[i]).rotate(tt.rotation, pos), (pos + vv[(i + 1) % vv.size()]).rotate(tt.rotation, pos));
		//	}
		//}

		//auto ff = (new_characters[1].get<components::pathfinding>().get_current_navigation_point() - position(new_characters[1])).set_length(15000);
		//new_characters[1].get<components::physics>().apply_force(ff);

		// LOG("F: %x", ff);
	}


	void testbed::view_cosmos(basic_viewing_step& step) const {
		auto& cosmos = step.cosm;
		
		viewing_step viewing(step, cosmos[world_camera].get<components::camera>().how_camera_will_render);
		rendering_scripts::standard_rendering(viewing);

		auto& target = step.renderer;
		using namespace gui::text;

		//quick_print_format(target.triangles, L"Be welcomed in Hypersomnia, Architect.", style(assets::font_id::GUI_FONT, violet), vec2i(200-1, 200), 0, nullptr);
		//quick_print_format(target.triangles, L"Be welcomed in Hypersomnia, Architect.", style(assets::font_id::GUI_FONT, violet), vec2i(200+1, 200), 0, nullptr);
		//quick_print_format(target.triangles, L"Be welcomed in Hypersomnia, Architect.", style(assets::font_id::GUI_FONT, violet), vec2i(200, 200 - 1), 0, nullptr);
		//quick_print_format(target.triangles, L"Be welcomed in Hypersomnia, Architect.", style(assets::font_id::GUI_FONT, violet), vec2i(200, 200+1), 0, nullptr);
		//

		auto coords = cosmos[characters[current_character]].get<components::transform>().pos;

		quick_print_format(target.triangles, typesafe_sprintf(L"X: %f2\nY: %f2\n", coords.x, coords.y) 
			+ cosmos.profiler.summary(show_profile_details), style(assets::GUI_FONT, rgba(255, 255, 255, 150)), vec2i(0, 0), 0, nullptr);
		target.call_triangles();
		target.clear_triangles();
	}
}