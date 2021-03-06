#if 0

#include <thread>
#include <mutex>
#include <array>
#include "augs/global_libraries.h"
#include "augs/window_framework/window.h"

#include "game/assets/assets_manager.h"

#include "game/hardcoded_content/test_scenes/testbed.h"

#include "game/transcendental/types_specification/all_component_includes.h"
#include "game/view/viewing_session.h"
#include "game/transcendental/step_packaged_for_network.h"
#include "game/transcendental/cosmos.h"
#include "game/transcendental/logic_step.h"
#include "game/transcendental/cosmic_movie_director.h"
#include "game/transcendental/types_specification/all_messages_includes.h"
#include "game/transcendental/data_living_one_step.h"

#include "augs/audio/sound_buffer.h"
#include "augs/audio/sound_source.h"

#include "augs/filesystem/file.h"
#include "augs/misc/action_list.h"
#include "augs/misc/standard_actions.h"
#include "main_menu_setup.h"

#include "augs/gui/text/caret.h"

#include "application/menu/menu_root.h"
#include "application/menu/menu_context.h"

#include "application/menu/appearing_text.h"
#include "application/menu/creators_screen.h"

#include "augs/misc/http_requests.h"
#include "augs/templates/string_templates.h"

#include "augs/graphics/drawers.h"
#include "game/detail/visible_entities.h"
#include "application/config_lua_table.h"
#include "augs/misc/lua_readwrite.h"

#include "augs/audio/sound_samples_from_file.h"
#include "generated/introspectors.h"

#include "application/setups/local_setup.h"

using namespace augs::event::keys;
using namespace augs::gui::text;
using namespace augs::gui;

void main_menu_setup::process(
	game_window& window,
	viewing_session& session
) {
	const auto metas_of_assets_unique = get_assets_manager().generate_logical_metas_of_assets();
	const auto& metas_of_assets = *metas_of_assets_unique;

	const vec2i screen_size = vec2i(window.get_screen_size());

	auto center = [&](auto& t) {
		t.target_pos = screen_size / 2 - get_text_bbox(t.get_total_target_text(), 0)*0.5f;
	};

	cosmos intro_scene(3000);

	auto& lua = augs::get_thread_local_lua_state();
	sol::table menu_config_patch = lua.script(augs::get_file_contents("content/menu/config.lua"), augs::lua_error_callback);

	augs::single_sound_buffer menu_theme;
	augs::sound_source menu_theme_source;

	float gain_fade_multiplier = 0.f;

	if (session.config.music_volume > 0.f) {
		if (augs::file_exists(session.config.menu_theme_path)) {
			menu_theme.set_data(augs::get_sound_samples_from_file(session.config.menu_theme_path));

			menu_theme_source.bind_buffer(menu_theme);
			menu_theme_source.set_direct_channels(true);
			menu_theme_source.seek_to(static_cast<float>(session.config.start_menu_music_at_secs));
			menu_theme_source.set_gain(0.f);
			menu_theme_source.play();
		}
	}

	const style textes_style = style(assets::font_id::GUI_FONT, cyan);

	std::mutex news_mut;

	bool draw_menu_gui = false;
	bool roll_news = false;
	text_drawer latest_news_drawer;
	vec2 news_pos = vec2(static_cast<float>(screen_size.x), 5.f);

	std::thread latest_news_query([&latest_news_drawer, &session, &textes_style, &news_mut]() {
		auto result = augs::http_get_request(session.config.latest_news_url);
		const std::string delim = "newsbegin";

		const auto it = result.find(delim);

		if (it == std::string::npos) {
			return;
		}

		result = result.substr(it + delim.length());

		str_ops(result)
			.multi_replace_all({ "\r", "\n" }, "")
			;

		if (result.size() > 0) {
			const auto wresult = to_wstring(result);

			std::unique_lock<std::mutex> lck(news_mut);
			latest_news_drawer.set_text(format_as_bbcode(wresult, textes_style));
		}
	});

	augs::fixed_delta_timer timer = augs::fixed_delta_timer(5);
	timer.set_stepping_speed_multiplier(1.f);

	// TODO: actually load a cosmos with its resources from a file/folder
	const bool is_intro_scene_available = session.config.menu_intro_scene_cosmos_path.size() > 0;

	if (is_intro_scene_available) {
		test_scenes::testbed().populate_world_with_entities(
			intro_scene,
			metas_of_assets,
			session.get_standard_post_solve()
		);
	}

	const auto character_in_focus = is_intro_scene_available ?
		intro_scene.get_entity_by_name(L"player0")
		: intro_scene[entity_id()]
	;

	const auto title_size = metas_of_assets[assets::game_image_id::MENU_GAME_LOGO].get_size();

	ltrb title_rect;
	title_rect.set_size(title_size);

	rgba fade_overlay_color = { 0, 2, 2, 255 };
	rgba title_text_color = { 255, 255, 255, 0 };


	rgba tweened_menu_button_color = cyan;
	tweened_menu_button_color.a = 0;

	vec2i tweened_welcome_message_bg_size;

	menu_rect_world menu_rect_world;
	menu_root_in_context menu_root_id;
	menu_root menu_root;

	for (auto& m : menu_root.buttons) {
		m.hover_highlight_maximum_distance = 10.f;
		m.hover_highlight_duration_ms = 300.f;
	}

	menu_root.buttons[main_menu_button_type::CONNECT_TO_OFFICIAL_UNIVERSE].set_appearing_caption(format(L"Login to official universe", textes_style));
	menu_root.buttons[main_menu_button_type::BROWSE_UNOFFICIAL_UNIVERSES].set_appearing_caption(format(L"Browse unofficial universes", textes_style));
	menu_root.buttons[main_menu_button_type::HOST_UNIVERSE].set_appearing_caption(format(L"Host universe", textes_style));
	menu_root.buttons[main_menu_button_type::CONNECT_TO_UNIVERSE].set_appearing_caption(format(L"Connect to universe", textes_style));
	menu_root.buttons[main_menu_button_type::LOCAL_UNIVERSE].set_appearing_caption(format(L"Local universe", textes_style));
	menu_root.buttons[main_menu_button_type::EDITOR].set_appearing_caption(format(L"Editor", textes_style));
	menu_root.buttons[main_menu_button_type::SETTINGS].set_appearing_caption(format(L"Settings", textes_style));
	menu_root.buttons[main_menu_button_type::CREATORS].set_appearing_caption(format(L"Founders", textes_style));
	menu_root.buttons[main_menu_button_type::QUIT].set_appearing_caption(format(L"Quit", textes_style));

	appearing_text credits1;
	credits1.target_text[0] = format(L"hypernet community presents", textes_style);
	center(credits1);

	appearing_text credits2;
	credits2.target_text = { format(L"A universe founded by\n", textes_style), format(L"Patryk B. Czachurski", textes_style) };
	center(credits2);

	std::vector<appearing_text*> intro_texts = { &credits1, &credits2 };

	appearing_text developer_welcome;
	{
		developer_welcome.population_interval = 60.f;

		developer_welcome.should_disappear = false;
		developer_welcome.target_text[0] = format(L"Thank you for building Hypersomnia.\n", textes_style);
		developer_welcome.target_text[1] = format(L"This message is not included in distributed executables.\n\
Our collective welcomes all of your suggestions and contributions.\n\
We wish you an exciting journey through architecture of our cosmos.\n", textes_style) +
		format(L"    ~hypernet community", style(assets::font_id::GUI_FONT, { 0, 180, 255, 255 }));
	}

	appearing_text hypersomnia_description;
	{
		hypersomnia_description.population_interval = 60.f;

		hypersomnia_description.should_disappear = false;
		hypersomnia_description.target_text[0] = format(L"- tendency of the omnipotent deity to immerse into inferior simulations,\nin spite of countless deaths experienced as a consequence.", { assets::font_id::GUI_FONT, {200, 200, 200, 255} });
	}

	std::vector<appearing_text*> title_texts = { &developer_welcome, &hypersomnia_description };

	vec2i tweened_menu_button_size;
	vec2i target_tweened_menu_button_size = menu_root.get_max_menu_button_size();

	creators_screen creators;

	augs::action_list intro_actions;

	{
		const bool play_credits = !session.config.skip_credits;

		if (play_credits) {
			intro_actions.push_blocking(act(new augs::delay_action(500.f)));
			intro_actions.push_non_blocking(act(new augs::tween_value_action<rgba_channel>(fade_overlay_color.a, 100, 6000.f)));
			intro_actions.push_non_blocking(act(new augs::tween_value_action<float>(gain_fade_multiplier, 1.f, 6000.f)));
			intro_actions.push_blocking(act(new augs::delay_action(2000.f)));
			
			for (auto& t : intro_texts) {
				t->push_actions(intro_actions);
			}
		}
		else {
			intro_actions.push_non_blocking(act(new augs::tween_value_action<float>(gain_fade_multiplier, 1.f, 6000.f)));
			fade_overlay_color.a = 100;
		}

		intro_actions.push_non_blocking(act(new augs::tween_value_action<rgba_channel>(fade_overlay_color.a, 20, 500.f)));

		intro_actions.push_blocking(act(new augs::tween_value_action<rgba_channel>(tweened_menu_button_color.a, 255, 250.f)));
		{
			augs::action_list welcome_tweens;

			const auto bbox = get_text_bbox(developer_welcome.get_total_target_text(), 0);
			
			welcome_tweens.push_blocking(act(new augs::tween_value_action<int>(tweened_welcome_message_bg_size.x, bbox.x, 500.f)));
			welcome_tweens.push_blocking(act(new augs::tween_value_action<int>(tweened_welcome_message_bg_size.y, bbox.y, 350.f)));
			
			intro_actions.push_non_blocking(act(new augs::list_action(std::move(welcome_tweens))));
		}

		intro_actions.push_blocking(act(new augs::tween_value_action<int>(tweened_menu_button_size.x, target_tweened_menu_button_size.x, 500.f)));
		intro_actions.push_blocking(act(new augs::tween_value_action<int>(tweened_menu_button_size.y, target_tweened_menu_button_size.y, 350.f)));

		intro_actions.push_non_blocking(act(new augs::tween_value_action<rgba_channel>(title_text_color.a, 255, 500.f)));
		
		intro_actions.push_non_blocking(act(new augs::set_value_action<bool>(roll_news, true)));
		intro_actions.push_non_blocking(act(new augs::set_value_action<vec2i>(menu_rect_world.last_state.mouse.pos, window.get_screen_size()/2)));
		intro_actions.push_non_blocking(act(new augs::set_value_action<bool>(draw_menu_gui, true)));
		
		for (auto& t : title_texts) {
			augs::action_list acts;
			t->push_actions(acts);

#if !IS_PRODUCTION_BUILD
			intro_actions.push_non_blocking(act(new augs::list_action(std::move(acts))));
#endif
		}

		for (auto& m : menu_root.buttons) {
			augs::action_list acts;
			m.appearing_caption.push_actions(acts);

			intro_actions.push_non_blocking(act(new augs::list_action(std::move(acts))));
		}
	}

	cosmic_movie_director director;
	director.load_recording_from_file(session.config.menu_intro_scene_entropy_path);
	
	const bool is_recording_available = is_intro_scene_available && director.is_recording_available();
	
	timer.reset_timer();

	const auto initial_step_number = intro_scene.get_total_steps_passed();

	augs::action_list credits_actions;

	auto menu_callback = [&](const main_menu_button_type t){
		switch (t) {

		case main_menu_button_type::QUIT:
			should_quit = true;
			break;

		case main_menu_button_type::SETTINGS:
			session.show_settings = true;
			ImGui::SetWindowFocus("Settings");
			break;

		case main_menu_button_type::CREATORS:
			if (credits_actions.is_complete()) {
				creators = creators_screen();
				creators.setup(textes_style, style(assets::font_id::GUI_FONT, { 0, 180, 255, 255 }), window.get_screen_size());

				credits_actions.push_blocking(act(new augs::tween_value_action<rgba_channel>(fade_overlay_color.a, 170, 500.f)));
				creators.push_into(credits_actions);
				credits_actions.push_blocking(act(new augs::tween_value_action<rgba_channel>(fade_overlay_color.a, 20, 500.f)));
			}
			break;

		case main_menu_button_type::LOCAL_UNIVERSE:
		{
			auto setup = std::make_unique<local_setup>();
			setup->process(window, session);

			if (setup->should_quit) {
				should_quit = true;
			}
		}
			break;

		default: break;
		}
	};

	if (is_recording_available) {
		while (intro_scene.get_total_time_passed_in_seconds() < session.config.rewind_intro_scene_by_secs) {
			const auto entropy = cosmic_entropy(director.get_entropy_for_step(intro_scene.get_total_steps_passed() - initial_step_number), intro_scene);

			intro_scene.advance_deterministic_schemata(
				{ entropy, metas_of_assets },
				[](auto) {},
				session.get_standard_post_solve()
			);
		}
	}

	while (!should_quit) {
		sync_config_back(session.config, window);
		
		DEBUG_DRAWING = session.config.debug_drawing;

		const auto screen_size = window.get_screen_size();
		augs::renderer::get_current().resize_fbos(screen_size);
		session.set_screen_size(screen_size);
		title_rect.set_position({ screen_size.x / 2.f - title_size.x / 2.f, 50.f });
		hypersomnia_description.target_pos = title_rect.left_bottom() + vec2(20, 20);
		menu_rect_world.last_state.screen_size = screen_size;
		developer_welcome.target_pos = screen_size - get_text_bbox(developer_welcome.get_total_target_text(), 0) - vec2(70.f, 70.f);

		augs::machine_entropy new_machine_entropy;

		{
			auto scope = measure_scope(get_profiler().local_entropy);
			new_machine_entropy.local = window.collect_entropy();
		}
		
		if (draw_menu_gui) {
			session.perform_imgui_pass(
				window,
				new_machine_entropy.local,
				session.imgui_timer.extract<std::chrono::milliseconds>()
			);
		}

		process_exit(new_machine_entropy.local);

		{
			auto translated = session.config.controls.translate(new_machine_entropy.local);
			session.fetch_developer_console_intents(translated.intents);
		}

		auto steps = timer.count_logic_steps_to_perform(intro_scene.get_fixed_delta());

		if (is_intro_scene_available) {
			while (steps--) {
				augs::renderer::get_current().clear_logic_lines();

				const auto entropy = is_recording_available ? 
					cosmic_entropy(director.get_entropy_for_step(intro_scene.get_total_steps_passed() - initial_step_number), intro_scene)
					: cosmic_entropy()
				;
				
				intro_scene.advance_deterministic_schemata(
					{ entropy, metas_of_assets },
					[](auto){},
					session.get_standard_post_solve()
				);
			}
		}

		const auto saved_config = session.config;
		augs::read(menu_config_patch, session.config);
		
		// treat as multiplier
		session.config.audio_volume.sound_effects *= saved_config.audio_volume.sound_effects;
		menu_theme_source.set_gain(session.config.audio_volume.music * gain_fade_multiplier);

		thread_local visible_entities all_visible;
		session.get_visible_entities(all_visible, intro_scene);

		const augs::delta vdt =
			timer.get_stepping_speed_multiplier()
			* session.frame_timer.extract<std::chrono::milliseconds>()
		;

		session.advance_audiovisual_systems(
			intro_scene, 
			character_in_focus, 
			all_visible,
			vdt
		);
		
		auto& renderer = augs::renderer::get_current();
		renderer.clear_current_fbo();

		const auto current_time_seconds = intro_scene.get_total_time_passed_in_seconds();

		session.view(
			renderer, 
			intro_scene,
			character_in_focus,
			all_visible,
			timer.fraction_of_step_until_next_step(intro_scene.get_fixed_delta()), 
			augs::gui::text::format(typesafe_sprintf(L"Current time: %x", current_time_seconds), textes_style)
		);

		session.config = saved_config;
		
		session.draw_color_overlay(renderer, fade_overlay_color);

		augs::draw_rect(
			renderer.get_triangle_buffer(),
			title_rect,
			assets::game_image_id::MENU_GAME_LOGO,
			title_text_color
		);

#if !IS_PRODUCTION_BUILD
		if (tweened_welcome_message_bg_size.non_zero()) {
			augs::draw_rect_with_border(renderer.get_triangle_buffer(),
				ltrb(developer_welcome.target_pos, tweened_welcome_message_bg_size).expand_from_center(vec2(6, 6)),
				{ 0, 0, 0, 180 },
				slightly_visible_white
			);
		}
#endif

		if (tweened_menu_button_size.non_zero()) {
			ltrb buttons_bg;
			buttons_bg.set_position(menu_root.buttons.front().rc.left_top());
			buttons_bg.b = menu_root.buttons.back().rc.b;
			buttons_bg.w(tweened_menu_button_size.x);

			augs::draw_rect_with_border(renderer.get_triangle_buffer(),
				buttons_bg.expand_from_center(vec2(14, 10)),
				{ 0, 0, 0, 180 },
				slightly_visible_white
			);
		}

		for (auto& t : intro_texts) {
			t->draw(renderer.get_triangle_buffer());
		}

		for (auto& t : title_texts) {
			t->draw(renderer.get_triangle_buffer());
		}

		creators.draw(renderer);

		if (roll_news) {
			news_pos.x -= vdt.in_seconds() * 100.f;

			{
				std::unique_lock<std::mutex> lck(news_mut);

				if (news_pos.x < -latest_news_drawer.get_bbox().x) {
					news_pos.x = static_cast<float>(screen_size.x);
				}

				latest_news_drawer.pos = news_pos;
				latest_news_drawer.draw_stroke(renderer.get_triangle_buffer());
				latest_news_drawer.draw(renderer.get_triangle_buffer());
			}
		}

		renderer.call_and_clear_triangles();
		
			menu_root.set_menu_buttons_positions(screen_size);
			menu_root.set_menu_buttons_sizes(tweened_menu_button_size);

			menu_rect_world::gui_entropy gui_entropies;

			menu_context::tree_type menu_tree;
			menu_context menu_context(session.config, menu_rect_world, menu_tree, menu_root);

			menu_rect_world.build_tree_data_into(menu_context, menu_root_id);

			vec2i cursor_drawing_pos;

			if (draw_menu_gui) {
				if (ImGui::GetIO().WantCaptureMouse) {
					/* unhover anything that was hovered */
					menu_rect_world.unhover_and_undrag(menu_context, gui_entropies);
					menu_rect_world.last_state.mouse.pos = ImGui::GetIO().MousePos;
				}
				else {
					for (const auto& ch : new_machine_entropy.local) {
						menu_rect_world.consume_raw_input_and_generate_gui_events(menu_context, menu_root_id, ch, gui_entropies);
					}
				
					ImGui::GetIO().MousePos = vec2(menu_rect_world.last_state.mouse.pos);
				
					menu_rect_world.call_idle_mousemotion_updater(menu_context, menu_root_id, gui_entropies);
				}

				cursor_drawing_pos = ImGui::GetIO().MousePos;
			}

			menu_rect_world.respond_to_events(menu_context, menu_root_id, gui_entropies);
			menu_rect_world.advance_elements(menu_context, menu_root_id, vdt);

			menu_root.set_menu_buttons_colors(tweened_menu_button_color);
			menu_rect_world.rebuild_layouts(menu_context, menu_root_id);

			menu_rect_world.draw(renderer.get_triangle_buffer(), menu_context, menu_root_id);

			for (size_t i = 0; i < menu_root.buttons.size(); ++i) {
				if (menu_root.buttons[i].click_callback_required) {
					menu_callback(static_cast<main_menu_button_type>(i));
					menu_root.buttons[i].click_callback_required = false;
				}
			}

			renderer.call_triangles();
			renderer.clear_triangles();

		renderer.draw_call_imgui(get_assets_manager());
		
		if (draw_menu_gui) {
			const auto gui_cursor_color = white;
		
			auto gui_cursor = assets::game_image_id::GUI_CURSOR;
		
			if (
				menu_context.alive(menu_rect_world.rect_hovered)
				|| ImGui::IsAnyItemHoveredWithHandCursor()
			) {
				gui_cursor = assets::game_image_id::GUI_CURSOR_HOVER;
			}

			if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE) {
				gui_cursor = assets::game_image_id::GUI_CURSOR_RESIZE_NWSE;
			}

			if (ImGui::GetMouseCursor() == ImGuiMouseCursor_TextInput) {
				gui_cursor = assets::game_image_id::GUI_CURSOR_TEXT_INPUT;
			}
			
			augs::draw_cursor(renderer.get_triangle_buffer(), cursor_drawing_pos, gui_cursor, gui_cursor_color);
			renderer.call_triangles();
			renderer.clear_triangles();
		}

		intro_actions.update(vdt);
		credits_actions.update(vdt);

		window.swap_buffers();
	}

	latest_news_query.detach();
}
#endif