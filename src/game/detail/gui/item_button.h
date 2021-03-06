#pragma once
#include "augs/gui/rect.h"
#include "augs/gui/text_drawer.h"
#include "augs/gui/appearance_detector.h"

#include "game/detail/inventory/inventory_slot_id.h"
#include "game/transcendental/entity_handle.h"
#include "game/assets/assets_manager.h"

#include "augs/pad_bytes.h"
#include "game/assets/assets_manager.h"

#include "game_gui_context.h"
#include "game/detail/gui/game_gui_context.h"

struct item_button : game_gui_rect_node {
	typedef augs::gui::draw_info draw_info;
	typedef game_gui_rect_node base;
	typedef base::gui_entropy gui_entropy;

	typedef dereferenced_location<item_button_in_item> this_in_item;
	typedef const_dereferenced_location<item_button_in_item> const_this_in_item;

	augs::gui::appearance_detector detector;

	bool is_container_open = false;
	bool started_drag = false;
	pad_bytes<2> pad;

	vec2i drag_offset_in_item_deposit;

	struct layout_with_attachments {
		augs::constant_size_vector<ltrb, 10> boxes;
		ltrb aabb;
		
		auto get_base_item_pos() const {
			return boxes[0];
		}

		void push(const ltrb l) {
			aabb.contain(l);
			boxes.push_back(l);
		}
	};

	static layout_with_attachments calculate_button_layout(
		const const_entity_handle component_owner,
		const bool include_attachments = true
	);

	struct drawing_settings {
		bool draw_background = false;
		bool draw_item = false;
		bool draw_attachments_even_if_open = false;
		bool draw_border = false;
		bool draw_connector = false;
		bool decrease_alpha = false;
		bool decrease_border_alpha = false;
		bool draw_container_opened_mark = false;
		bool draw_charges = true;
		bool expand_size_to_grid = true;
		bool always_full_item_alpha = false;
		vec2 absolute_xy_offset;
	};

	item_button(xywh rc = xywh());

	template <class C, class gui_element_id, class L>
	static void for_each_child(const C context, const gui_element_id this_id, L generic_call) {
		const auto container = context.get_cosmos()[this_id.get_location().item_id];

		if (container.has<components::container>()) {
			for (const auto& s : container.get<components::container>().slots) {
				{
					slot_button_in_container child_slot_location;
					child_slot_location.slot_id.type = s.first;
					child_slot_location.slot_id.container_entity = container;
					generic_call(context.dereference_location(child_slot_location));
				}

				for (const auto& in : s.second.items_inside) {
					item_button_in_item child_item_location;
					child_item_location.item_id = in;
					generic_call(context.dereference_location(child_item_location));
				}
			}
		}
	}

	static vec2 griddify_size(const vec2 size, const vec2 expander);


	static bool is_being_wholely_dragged_or_pending_finish(const const_game_gui_context, const const_this_in_item this_id);

	static void respond_to_events(const game_gui_context, const this_in_item this_id, const gui_entropy& entropies);
	static void rebuild_layouts(const game_gui_context, const this_in_item this_id);

	static bool is_inventory_root(const const_game_gui_context, const const_this_in_item this_id);
	
	static void draw(
		const viewing_game_gui_context, 
		const const_this_in_item this_id, 
		draw_info
	);

	static void draw_proc(
		const viewing_game_gui_context, 
		const const_this_in_item, 
		draw_info, 
		const drawing_settings&
	);

	static void draw_dragged_ghost_inside(
		const viewing_game_gui_context context, 
		const const_this_in_item this_id, 
		draw_info in,
		const vec2 absolute_xy_offset
	);

	static void draw_grid_border_ghost(
		const viewing_game_gui_context, 
		const const_this_in_item, 
		draw_info in,
		const vec2 absolute_xy_offset
	);

	static void draw_complete_dragged_ghost(
		const viewing_game_gui_context, 
		const const_this_in_item, 
		draw_info,
		const vec2 absolute_xy_offset
	);

	static void draw_complete_with_children(const viewing_game_gui_context, const const_this_in_item this_id, augs::gui::draw_info in);

};