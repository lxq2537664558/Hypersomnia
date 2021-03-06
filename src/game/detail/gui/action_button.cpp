#include "action_button.h"
#include "augs/gui/text_drawer.h"
#include "augs/gui/stroke.h"
#include "game/detail/gui/game_gui_context.h"
#include "game/transcendental/cosmos.h"
#include "game/systems_audiovisual/game_gui_system.h"
#include "game/components/sentience_component.h"
#include "game/assets/assets_manager.h"

#include "augs/graphics/drawers.h"
#include "augs/templates/visit_gettable.h"

void action_button::draw(
	const viewing_game_gui_context context,
	const const_this_in_item this_id,
	draw_info info
) {
	const auto intent_for_this = static_cast<intent_type>(static_cast<int>(intent_type::SPECIAL_ACTION_BUTTON_1) + this_id.get_location().index);
	const auto bound_key = context.get_input_information().get_bound_key_if_any(intent_for_this);

	const auto& cosmos = context.get_cosmos();
	
	const auto now = cosmos.get_timestamp();
	const auto dt = cosmos.get_fixed_delta();

	const auto absolute_rect = context.get_tree_entry(this_id).get_absolute_rect();

	if (bound_key != augs::event::keys::key::INVALID) {
		const auto& sentience = context.get_subject_entity().get<components::sentience>();
		const auto bound_spell = get_bound_spell(context, this_id);

		if (bound_spell.is_set()) {
			visit_gettable(
				sentience.spells,
				bound_spell,
				[&](const auto& spell){
					const auto spell_data = get_meta_of(spell, cosmos.get_global_state().spells);

					const auto& pe = sentience.get<personal_electricity_meter_instance>();
					const bool has_enough_mana = pe.value >= spell_data.common.personal_electricity_required;
					const float required_mana_ratio = std::min(1.f, pe.value / static_cast<float>(spell_data.common.personal_electricity_required));

					rgba inside_col = white;

					inside_col.a = 220;

					const auto& detector = this_id->detector;
					const bool is_pushed = detector.current_appearance == augs::gui::appearance_detector::appearance::pushed;

					if (detector.is_hovered) {
						inside_col.a = 255;
					}

					assets::game_image_id inside_tex = assets::game_image_id::INVALID;
					assets::game_image_id border_tex = assets::game_image_id::SPELL_BORDER;

					rgba border_col;

					const auto& manager = get_assets_manager();

					inside_tex = spell_data.appearance.icon;
					border_col = spell_data.common.associated_color;

					if (!has_enough_mana) {
						border_col = border_col.get_desaturated();
					}

					if (inside_tex != assets::game_image_id::INVALID) {
						ensure(border_tex != assets::game_image_id::INVALID);

						const augs::gui::material inside_mat(inside_tex, inside_col);

						const auto absolute_icon_rect = ltrbi(vec2i(0, 0), manager.at(inside_tex).get_size()).place_in_center_of(absolute_rect);
						const bool draw_partial_colorful_rect = false;

						if (has_enough_mana) {
							draw_clipped_rect(
								inside_mat,
								absolute_icon_rect,
								context,
								context.get_tree_entry(this_id).get_parent(),
								info.v
							);
						}
						else {
							augs::draw_clipped_rect(
								info.v,
								absolute_icon_rect,
								get_assets_manager()[inside_mat.tex].texture_maps[texture_map_type::DESATURATED],
								inside_mat.color,
								ltrbi()
							);

							if (draw_partial_colorful_rect) {
								auto colorful_rect = absolute_icon_rect;
								const auto colorful_height = static_cast<int>(absolute_icon_rect.h() * required_mana_ratio);
								colorful_rect.t = absolute_icon_rect.b - colorful_height;
								colorful_rect.b = colorful_rect.t + colorful_height;

								augs::draw_clipped_rect(
									info.v,
									absolute_icon_rect,
									get_assets_manager()[inside_mat.tex].texture_maps[texture_map_type::DIFFUSE],
									inside_mat.color,
									colorful_rect
								);
							}
						}

						bool is_still_cooled_down = false;

						{
							const auto all_cooldown = sentience.cast_cooldown_for_all_spells;
							const auto this_cooldown = spell.cast_cooldown;

							const auto effective_cooldown_ratio =
								all_cooldown.get_remaining_time_ms(now, dt) > this_cooldown.get_remaining_time_ms(now, dt)
								?
								all_cooldown.get_ratio_of_remaining_time(now, dt) : this_cooldown.get_ratio_of_remaining_time(now, dt);

							if (effective_cooldown_ratio > 0.f) {
								augs::draw_rectangle_clock(info.v, effective_cooldown_ratio, absolute_icon_rect, { 0, 0, 0, 200 });
								is_still_cooled_down = true;
							}
						}
						
						if (is_still_cooled_down) {
							border_col.a -= 150;
						}

						if (this_id->detector.is_hovered) {
							border_col = rgba(220, 220, 220, 255);
						}

						if (is_pushed) {
							border_col = white;
						}

						const augs::gui::material border_mat(border_tex, border_col);

						draw_centered_texture(context, this_id, info, border_mat);

						auto label_col = has_enough_mana ? border_col : white;
						label_col.a = 255;
						const auto label_style = augs::gui::text::style(assets::font_id::GUI_FONT, label_col );

						augs::gui::text_drawer bound_key_caption;

						bound_key_caption.set_text(
							augs::gui::text::format(
								key_to_wstring(bound_key),
								label_style
							)
						);

						bound_key_caption.bottom_right(absolute_rect);
						bound_key_caption.pos.x -= 4;
						bound_key_caption.draw_stroke(info.v);
						bound_key_caption.draw(info.v);
					}
				}
			);
		}
		else {
			rgba inside_col, border_col;

			inside_col = cyan;

			border_col = inside_col;
			inside_col.a = 4 * 5;
			border_col.a = 220;

			if (this_id->detector.is_hovered || this_id->detector.current_appearance == augs::gui::appearance_detector::appearance::pushed) {
				inside_col.a = 12 * 5;
				border_col.a = 255;
			}

			const auto label_style = augs::gui::text::style(assets::font_id::GUI_FONT, border_col);

			const auto inside_tex = assets::game_image_id::ACTION_BUTTON_FILLED;
			const auto border_tex = assets::game_image_id::ACTION_BUTTON_BORDER;

			const augs::gui::material inside_mat(inside_tex, inside_col);
			const augs::gui::material border_mat(border_tex, border_col);

			draw_centered_texture(context, this_id, info, inside_mat);
			draw_centered_texture(context, this_id, info, border_mat);

			augs::gui::text_drawer bound_key_caption;

			bound_key_caption.set_text(
				augs::gui::text::format(
					key_to_wstring(bound_key),
					label_style
				)
			);

			bound_key_caption.center(absolute_rect);
			bound_key_caption.pos.y += 2;
			bound_key_caption.draw_stroke(info.v);
			bound_key_caption.draw(info.v);
		}
	}
}

spell_id action_button::get_bound_spell(
	const const_game_gui_context context,
	const const_this_in_item this_id
) {
	const auto bound_spell = this_id->bound_spell;
	const auto& sentience = context.get_subject_entity().get<components::sentience>();

	if (bound_spell.is_set() && sentience.is_learned(bound_spell)) {
		return bound_spell;
	}

	return spell_id();
}

void action_button::advance_elements(
	const game_gui_context context,
	const this_in_item this_id,
	const augs::delta dt
) {
	base::advance_elements(context, this_id, dt);

	if (this_id->detector.is_hovered) {
		this_id->elapsed_hover_time_ms += dt.in_milliseconds();
	}

	if (this_id->detector.current_appearance == augs::gui::appearance_detector::appearance::pushed) {
		this_id->elapsed_hover_time_ms = this_id->hover_highlight_duration_ms;
	}
}

void action_button::respond_to_events(
	const game_gui_context context,
	const this_in_item this_id,
	const gui_entropy& entropies
) {
	base::respond_to_events(context, this_id, entropies);

	for (const auto& info : entropies.get_events_for(this_id)) {
		this_id->detector.update_appearance(info);

		if (info.msg == gui_event::lclick) {
			const auto bound_spell = get_bound_spell(context, this_id);
			
			if (bound_spell.is_set()) {
				context.get_game_gui_system().spell_requests[context.get_subject_entity()] = bound_spell;
			}
		}

		if (info.msg == gui_event::hover) {
			this_id->elapsed_hover_time_ms = 0.f;
		}

		if (info.msg == gui_event::lfinisheddrag) {
		}
	}
}

void action_button::rebuild_layouts(
	const game_gui_context context,
	const this_in_item this_id
) {
	base::rebuild_layouts(context, this_id);
}