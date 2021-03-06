#pragma once
#include "augs/gui/appearance_detector.h"
#include "augs/gui/material.h"

#include "augs/pad_bytes.h"

#include "game/assets/game_image_id.h"

#include "augs/gui/button_corners.h"
#include "application/menu/appearing_text.h"

#include "augs/audio/sound_source.h"

template <class Enum>
class option_button : public menu_rect_node<Enum> {
public:
	augs::gui::appearance_detector detector;

	augs::sound_source hover_sound;
	augs::sound_source click_sound;

	appearing_text appearing_caption;
	button_corners_info corners;
	float elapsed_hover_time_ms = 0.f;

	float hover_highlight_maximum_distance = 8.f;
	float hover_highlight_duration_ms = 400.f;

	rgba colorize;
	bool click_callback_required = false;
	pad_bytes<3> pad;

	using base_node = menu_rect_node<Enum>;
	using gui_entropy = typename base_node::gui_entropy;

	option_button() {
		corners.inside_texture = assets::game_image_id::MENU_BUTTON_INSIDE;
	}

	vec2i get_target_button_size() const {
		return corners.internal_size_to_cornered_size(get_text_bbox(appearing_caption.get_total_target_text(), 0)) - vec2i(0, 3);
	}

	void set_complete_caption(const augs::gui::text::formatted_string& text) {
		set_appearing_caption(text);
		make_complete();
	}

	void set_appearing_caption(const augs::gui::text::formatted_string& text) {
		appearing_caption.population_interval = 100.f;

		appearing_caption.should_disappear = false;
		appearing_caption.target_text[0] = text;
	}

	void make_complete() {
		appearing_caption.text = appearing_caption.target_text[0];
		appearing_caption.alpha = 255;
	}

	template <class C, class D>
	static void advance_elements(const C context, const D this_id, const augs::delta dt) {
		this_id->click_sound.set_gain(context.audio_volume.gui);
		this_id->hover_sound.set_gain(context.audio_volume.gui);

		if (this_id->detector.is_hovered) {
			this_id->elapsed_hover_time_ms += dt.in_milliseconds();
		}
	}

	template <class C, class D>
	static void respond_to_events(const C context, const D this_id, const gui_entropy& entropies) {
		const auto& manager = get_assets_manager();

		for (const auto& info : entropies.get_events_for(this_id)) {
			this_id->detector.update_appearance(info);

			if (info.is_ldown_or_double_or_triple()) {
				this_id->click_callback_required = true;
				this_id->click_sound.stop();
				this_id->click_sound.bind_buffer(manager.at(assets::sound_buffer_id::BUTTON_CLICK).get_variation(0).request_original());
				this_id->click_sound.set_direct_channels(true);
				this_id->click_sound.play();
			}
			if (info.msg == gui_event::hover) {
				this_id->elapsed_hover_time_ms = 0.f;
				this_id->hover_sound.stop();
				this_id->hover_sound.bind_buffer(manager.at(assets::sound_buffer_id::BUTTON_HOVER).get_variation(0).request_original());
				this_id->hover_sound.set_direct_channels(true);
				this_id->hover_sound.play();
			}
		}
	}

	template <class C, class D>
	static void draw(const C context, const D this_id, augs::gui::draw_info in) {
		if (!this_id->get_flag(augs::gui::flag::ENABLE_DRAWING)) {
			return;
		}

		const auto& rect_world = context.get_rect_world();
		const auto& this_tree_entry = context.get_tree_entry(this_id);

		const auto& detector = this_id->detector;

		rgba inside_col = white;
		rgba border_col = white;

		inside_col.a = 20;
		border_col.a = 190;

		if (detector.is_hovered) {
			inside_col.a = 30;
			border_col.a = 220;
		}

		const bool pushed = detector.current_appearance == augs::gui::appearance_detector::appearance::pushed;

		if (pushed) {
			inside_col.a = 60;
			border_col.a = 255;

			this_id->elapsed_hover_time_ms = this_id->hover_highlight_duration_ms;
		}

		inside_col *= this_id->colorize;
		border_col *= this_id->colorize;

		const bool flip = this_id->corners.flip_horizontally;
		const auto internal_rc = this_id->corners.cornered_rc_to_internal_rc(this_tree_entry.get_absolute_rect());

		{
			this_id->corners.for_each_button_corner(internal_rc, [&](const button_corner_type type, const assets::game_image_id id, const ltrb drawn_rc) {
				const auto col = is_button_border(type) ? border_col : inside_col;
				augs::gui::draw_clipped_rect(augs::gui::material(id, col), drawn_rc, {}, in.v, flip);
			});

			if (this_id->detector.is_hovered) {
				auto hover_effect_rc = internal_rc;

				if (pushed) {
					const auto distance = 4.f;
					hover_effect_rc.expand_from_center(vec2(distance, distance));

					this_id->corners.for_each_button_corner(hover_effect_rc, [&](const button_corner_type type, const assets::game_image_id id, const ltrb drawn_rc) {
						if (is_button_border(type)) {
							augs::gui::draw_clipped_rect(augs::gui::material(id, this_id->colorize), drawn_rc, {}, in.v, flip);
						}
					});
				}
				else {
					const auto max_duration = this_id->hover_highlight_duration_ms;
					const auto max_distance = this_id->hover_highlight_maximum_distance;

					const auto distance = (1.f - std::min(max_duration, this_id->elapsed_hover_time_ms) / max_duration) * max_distance;
					hover_effect_rc.expand_from_center(vec2(distance, distance));

					this_id->corners.for_each_button_corner(hover_effect_rc, [&](const button_corner_type type, const assets::game_image_id id, const ltrb drawn_rc) {
						if (is_button_corner(type) && is_button_border(type)) {
							augs::gui::draw_clipped_rect(augs::gui::material(id, this_id->colorize), drawn_rc, {}, in.v, flip);
						}
					});
				}
			}
		}

		this_id->appearing_caption.target_pos = internal_rc.left_top();
		this_id->appearing_caption.draw(in.v);
	}
};