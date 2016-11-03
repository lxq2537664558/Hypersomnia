#pragma once
#include "game/detail/gui/gui_element_location.h"

class root_of_inventory_gui : public game_gui_rect_node {
public:
	typedef root_of_inventory_gui_location location;

	root_of_inventory_gui(const vec2 screen_size);

	template <class C, class gui_element_id, class L>
	static void for_each_child(C context, const gui_element_id& this_id, L generic_call) {
		const auto& handle = context.get_gui_element_entity();

		// we do not dereference the gui element's entity location because it is possibly not an item;
		// however it should be a container so we call the callback on the element's children
		// i.e. the player has a gui element component and container component but not an item component.
		item_button::for_each_child(context, item_button::location{ handle.get_id() }, generic_call);

		context(drag_and_drop_target_drop_item::location(), [&](const auto& dereferenced) {
			generic_call(dereferenced);
		});
	}
};