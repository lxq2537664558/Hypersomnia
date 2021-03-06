#pragma once
#include <variant>

#include "augs/gui/rect_world.h"

#include "game/detail/gui/locations/slot_button_in_container.h"
#include "game/detail/gui/locations/item_button_in_item.h"
#include "game/detail/gui/locations/drag_and_drop_target_drop_item_in_character_gui.h"
#include "game/detail/gui/locations/hotbar_button_in_character_gui.h"
#include "game/detail/gui/locations/action_button_in_character_gui.h"
#include "game/detail/gui/locations/game_gui_root_in_context.h"
#include "game/detail/gui/locations/value_bar_in_character_gui.h"

using game_gui_element_location = std::variant<
	item_button_in_item,
	slot_button_in_container,
	drag_and_drop_target_drop_item_in_character_gui,
	hotbar_button_in_character_gui,
	action_button_in_character_gui,
	game_gui_root_in_context,
	value_bar_in_character_gui
>;

using game_gui_rect_world = augs::gui::rect_world<game_gui_element_location>;
using game_gui_rect_node = augs::gui::rect_node<game_gui_element_location>;
