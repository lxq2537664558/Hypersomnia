#pragma once
#include <array>
#include "container_component.h"
#include "transform_component.h"
#include "game/detail/inventory/inventory_slot_id.h"
#include "game/transcendental/entity_handle_declaration.h"

#include "game/enums/item_category.h"

namespace components {
	struct item {
		enum mounting_state : unsigned char {
			MOUNTED,
			UNMOUNTED,
		};

		// GEN INTROSPECTOR struct components::item
		mounting_state current_mounting = UNMOUNTED;
		mounting_state intended_mounting = UNMOUNTED;
		bool stackable = false;
		pad_bytes<1> pad;

		item_category_flagset categories_for_slot_compatibility = { item_category::GENERAL };

		int charges = 1;
		unsigned space_occupied_per_charge = 1;

		unsigned dual_wield_accuracy_loss_percentage = 50;
		unsigned dual_wield_accuracy_loss_multiplier = 1;

		inventory_slot_id current_slot;
		inventory_slot_id target_slot_after_unmount;

		float montage_time_ms = 1000;
		float montage_time_left_ms = 0.f;
		// END GEN INTROSPECTOR

		void set_mounted();
		void request_mount();
		void cancel_montage();
		void request_unmount();
		void request_unmount(inventory_slot_id target_slot_after_unmount);
		void reset_mounting_timer();

		void mark_parent_enclosing_containers_for_unmount();

		unsigned get_space_occupied() const;
		bool is_mounted() const;
		bool are_parents_last_in_lifo_slots() const;
	};
}