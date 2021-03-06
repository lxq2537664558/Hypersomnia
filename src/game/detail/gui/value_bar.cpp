#include "drag_and_drop_target_drop_item.h"
#include "game/detail/gui/character_gui.h"
#include "game/detail/gui/character_gui.h"
#include "game/components/sentience_component.h"
#include "game/detail/gui/game_gui_root.h"
#include "game/detail/inventory/inventory_slot.h"
#include "game/detail/inventory/inventory_slot_handle.h"
#include "game/transcendental/entity_handle.h"
#include "game/transcendental/cosmos.h"
#include "game/systems_audiovisual/game_gui_system.h"
#include "augs/gui/text_drawer.h"
#include "augs/gui/stroke.h"
#include "augs/templates/visit_gettable.h"

static constexpr std::size_t num_sentience_meters = num_types_in_list_v<decltype(components::sentience::meters)>;

template <class F, class G>
decltype(auto) visit_by_vertical_index(
	const components::sentience& sentience,
	const cosmos& cosm,
	const unsigned index, 
	F&& meter_callback,
	G&& perk_callback
) {
	if (index < num_sentience_meters) {
		meter_id id;
		id.set_index(index);

		return visit_gettable(sentience.meters, id, 
			[&cosm, &meter_callback](const auto& meter){
				return meter_callback(meter, get_meta_of(meter, cosm.get_global_state().meters));
			}
		);
	}

	{
		perk_id id;
		id.set_index(index - num_sentience_meters);

		return visit_gettable(sentience.perks, id, 
			[&cosm, &perk_callback](const auto& perk){
				return perk_callback(perk, get_meta_of(perk, cosm.get_global_state().perks));
			}
		);
	}
}

template <class F>
decltype(auto) visit_by_vertical_index(
	const components::sentience& sentience,
	const cosmos& cosm,
	const unsigned index, 
	F&& callback
) {
	return visit_by_vertical_index(
		sentience,
		cosm,
		index,
		std::forward<F>(callback),
		std::forward<F>(callback)
	);
}

bool value_bar::is_sentience_meter(const const_this_pointer this_id) {
	return this_id.get_location().vertical_index < num_sentience_meters;
}

std::wstring value_bar::get_description_for_hover(
	const const_game_gui_context context,
	const const_this_pointer self
) {
	const auto& cosmos = context.get_cosmos();
	const auto& metas = cosmos.get_global_state();
	const auto& sentience = context.get_subject_entity().get<components::sentience>();

	return visit_by_vertical_index(
		context.get_subject_entity().get<components::sentience>(),
		cosmos,
		self.get_location().vertical_index,
		
		[&](const auto& meter, const auto& meta){
			return typesafe_sprintf(meta.appearance.get_description(), meter.get_value(), meter.get_maximum_value());
		},

		[&](const auto& perk, const auto& meta){
			return typesafe_sprintf(meta.appearance.get_description());
		}
	);
}

value_bar::value_bar() {
	unset_flag(augs::gui::flag::CLIP);
	set_flag(augs::gui::flag::ENABLE_DRAWING);
}

void value_bar::draw(
	const viewing_game_gui_context context, 
	const const_this_pointer this_id, 
	const augs::gui::draw_info info
) {
	const auto& cosmos = context.get_cosmos();
	const auto dt = cosmos.get_fixed_delta();
	const auto now = cosmos.get_timestamp();
	const auto& manager = get_assets_manager();

	if (!this_id->get_flag(augs::gui::flag::ENABLE_DRAWING)) {
		return;
	}

	auto icon_mat = get_icon_mat(context, this_id);

	if (this_id->detector.is_hovered) {
		icon_mat.color.a = 255;
	}
	else {
		icon_mat.color.a = 200;
	}

	const auto& tree_entry = context.get_tree_entry(this_id);
	const auto absolute = tree_entry.get_absolute_rect();

	ltrb icon_rect;
	icon_rect.set_position(absolute.get_position());
	icon_rect.set_size(manager.at(icon_mat.tex).get_size());

	draw_clipped_rect(
		icon_mat, 
		icon_rect, 
		context, 
		tree_entry.get_parent(), 
		info.v
	);

	const auto total_spacing = this_id->get_total_border_expansion();

	{
		const auto full_bar_rect_bordered = get_bar_rect_with_borders(context, this_id, absolute);
		const auto value_bar_rect = get_value_bar_rect(context, this_id, absolute);

		auto bar_mat = get_bar_mat(context, this_id);
		bar_mat.color.a = icon_mat.color.a;

		const auto vertical_index = this_id.get_location().vertical_index;
			
		const auto& sentience = context.get_subject_entity().get<components::sentience>();
		
		const auto current_value_ratio = visit_by_vertical_index(
			sentience,
			cosmos,
			vertical_index,

			[](const auto& meter, auto){
				return meter.get_ratio();
			},

			[now, dt](const auto& perk, auto){
				return perk.timing.get_ratio(now, dt);
			}
		);

		auto current_value_bar_rect = value_bar_rect;
		const auto bar_width = static_cast<int>(current_value_bar_rect.w() * current_value_ratio);
		current_value_bar_rect.w(static_cast<float>(bar_width));

		draw_clipped_rect(
			bar_mat,
			current_value_bar_rect,
			context,
			context.get_tree_entry(this_id).get_parent(),
			info.v
		);

		augs::gui::solid_stroke stroke;
		stroke.set_material(bar_mat);
		stroke.set_width(this_id->border_width);
		stroke.draw(info.v, value_bar_rect, ltrb(), this_id->border_spacing);

		meter_id id;

		if (is_sentience_meter(this_id)) {
			id.set_index(vertical_index);

			const auto value = visit_gettable(
				sentience.meters,
				id,
				[](const auto& meter) {
					return meter.get_value();
				}
			);

			augs::gui::text_drawer drawer;

			drawer.set_text(augs::gui::text::format(typesafe_sprintf(L"%x", static_cast<int>(value)), { assets::font_id::GUI_FONT, white }));
			drawer.pos.set(full_bar_rect_bordered.r + total_spacing*2, full_bar_rect_bordered.t - total_spacing);
			drawer.draw_stroke(info.v);
			drawer.draw(info.v);
		}

		if (bar_width >= 1) {
			for (const auto& p : this_id->particles) {
				auto particle_mat = p.mat;
				particle_mat.color = bar_mat.color + rgba(30, 30, 30, 0);
			
				const auto particle_rect = ltrb(value_bar_rect.get_position() - vec2(6, 6) + p.relative_pos, manager.at(particle_mat.tex).get_size());

				draw_clipped_rect(
					particle_mat,
					particle_rect,
					current_value_bar_rect,
					info.v
				);
			}
		}

		if (id.is<consciousness_meter_instance>()) {
			const auto boundary_mat = augs::gui::material();
			auto boundary_rect = ltrb();
			boundary_rect.l = value_bar_rect.l + value_bar_rect.w() / 10;
			boundary_rect.t = full_bar_rect_bordered.t;
			boundary_rect.b = full_bar_rect_bordered.b;
			boundary_rect.r = boundary_rect.l + 1;

			draw_clipped_rect(
				boundary_mat,
				boundary_rect,
				context,
				context.get_tree_entry(this_id).get_parent(),
				info.v
			);
		}
	}
}

ltrb value_bar::get_bar_rect_with_borders(
	const const_game_gui_context context,
	const const_this_pointer this_id,
	const ltrb absolute
) {
	const auto& manager = get_assets_manager();

	auto icon_rect = absolute;

	auto icon_mat = get_icon_mat(context, this_id);
	icon_rect.set_size(manager.at(icon_mat.tex).get_size());
	augs::gui::text_drawer drawer;
	drawer.set_text(augs::gui::text::format(L"99999", assets::font_id::GUI_FONT));

	const auto max_value_caption_size = drawer.get_bbox();

	auto value_bar_rect = icon_rect;
	value_bar_rect.set_position(icon_rect.get_position() + vec2i(this_id->get_total_border_expansion() + icon_rect.get_size().x, 0));
	value_bar_rect.r = absolute.r - max_value_caption_size.x;

	return value_bar_rect;
}

ltrb value_bar::get_value_bar_rect(
	const const_game_gui_context context,
	const const_this_pointer this_id,
	const ltrb absolute
) {
	const auto border_expansion = this_id->get_total_border_expansion();
	return get_bar_rect_with_borders(context, this_id, absolute).expand_from_center({ static_cast<float>(-border_expansion), static_cast<float>(-border_expansion) });
}

void value_bar::advance_elements(
	const game_gui_context context,
	const this_pointer this_id,
	const augs::delta dt
) {
	this_id->seconds_accumulated += dt.in_seconds();

	if (this_id->particles.size() > 0) {
		randomization rng(this_id.get_location().vertical_index + context.get_cosmos().get_total_time_passed_in_seconds() * 1000);

		const auto value_bar_size = get_value_bar_rect(context, this_id, this_id->rc).get_size();

		while (this_id->seconds_accumulated > 0.f) {
			for (auto& p : this_id->particles) {
				const auto action = rng.randval(0, 8);

				if (action == 0) {
					const auto y_dir = rng.randval(0, 1);

					if (y_dir == 0) {
						++p.relative_pos.y;
					}
					else {
						--p.relative_pos.y;
					}
				}
				
				++p.relative_pos.x;

				p.relative_pos.y = std::max(0, p.relative_pos.y);
				p.relative_pos.x = std::max(0, p.relative_pos.x);

				p.relative_pos.x %= static_cast<int>(value_bar_size.x + 12);
				p.relative_pos.y %= static_cast<int>(value_bar_size.y + 12);
			}

			this_id->seconds_accumulated -= 1.f / 15;
		}
	}
}

void value_bar::respond_to_events(
	const game_gui_context context, 
	const this_pointer this_id, 
	const gui_entropy& entropies
) {
	for (const auto& e : entropies.get_events_for(this_id)) {
		this_id->detector.update_appearance(e);
	}
}

augs::gui::material value_bar::get_icon_mat(
	const const_game_gui_context context, 
	const const_this_pointer this_id
) {
	const auto& cosmos = context.get_cosmos();
	const auto& metas = cosmos.get_global_state();
	const auto& sentience = context.get_subject_entity().get<components::sentience>();

	return { visit_by_vertical_index(
		sentience,
		cosmos,
		this_id.get_location().vertical_index,
		[](const auto& perk_or_meter, const auto& meta){
			return meta.appearance.get_icon();
		}
	), white };
}

augs::gui::material value_bar::get_bar_mat(
	const const_game_gui_context context, 
	const const_this_pointer this_id
) {
	const auto& cosmos = context.get_cosmos();
	const auto& metas = cosmos.get_global_state();
	const auto& sentience = context.get_subject_entity().get<components::sentience>();

	return { 
		context.get_game_gui_system().value_bar_background, 
		visit_by_vertical_index(
			sentience,
			cosmos,
			this_id.get_location().vertical_index,
			[](auto, const auto& meta){
				return meta.appearance.get_bar_color();
			}
		) 
	};
}

bool value_bar::is_enabled(
	const const_game_gui_context context, 
	const unsigned vertical_index
) {
	const auto& cosm = context.get_cosmos();

	const auto dt = cosm.get_fixed_delta();
	const auto now = cosm.get_timestamp();

	return 		
		visit_by_vertical_index(
			context.get_subject_entity().get<components::sentience>(),
			cosm,
			vertical_index,

			[](const auto& meter, auto){
				return meter.is_enabled();
			},

			[now, dt](const auto& perk, auto){
				return perk.timing.is_enabled(now, dt);
			}
		)
	;
}

void value_bar::rebuild_layouts(
	const game_gui_context context,
	const this_pointer this_id
) {
	const auto vertical_index = this_id.get_location().vertical_index;
	const auto& sentience = context.get_subject_entity().get<components::sentience>();

	const auto& cosmos = context.get_cosmos();
	const auto& manager = get_assets_manager();

	const auto dt = cosmos.get_fixed_delta();
	const auto now = cosmos.get_timestamp();

	if (!is_enabled(context, vertical_index)) {
		this_id->unset_flag(augs::gui::flag::ENABLE_DRAWING);
		return;
	}
	else {
		this_id->set_flag(augs::gui::flag::ENABLE_DRAWING);
	}

	unsigned drawing_vertical_index = 0;

	for (unsigned i = 0; i < vertical_index; ++i) {
		if (is_enabled(context, i)) {
			++drawing_vertical_index;
		}
	}

	const auto screen_size = context.get_character_gui().get_screen_size();
	const auto icon_size = manager.at(get_icon_mat(context, this_id).tex).get_size();
	const auto with_bar_size = vec2i(icon_size.x + 4 + 180, icon_size.y);

	const auto lt = vec2i(screen_size.x - 220, 20 + drawing_vertical_index * (icon_size.y + 4));

	this_id->rc.set_position(lt);
	this_id->rc.set_size(with_bar_size);

	if (this_id->particles.empty()) {
		randomization rng(this_id.get_location().vertical_index);

		const auto value_bar_size = get_value_bar_rect(context, this_id, this_id->rc).get_size();

		for (size_t i = 0; i < 40; ++i) {
			const augs::gui::material mats[3] = {
				assets::game_image_id::WANDERING_CROSS,
				assets::game_image_id::BLINK_1,
				static_cast<assets::game_image_id>(static_cast<int>(assets::game_image_id::BLINK_1) + 2),
			};

			effect_particle new_part;
			new_part.relative_pos = rng.randval(vec2(0, 0), value_bar_size);
			new_part.mat = mats[rng.randval(0, 2)];

			this_id->particles.push_back(new_part);
		}
	}
}