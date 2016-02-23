#include "ingredients.h"
#include "entity_system/entity.h"
#include "entity_system/world.h"

#include "game_framework/components/container_component.h"
#include "game_framework/components/item_component.h"
#include "game_framework/components/trigger_component.h"
#include "game_framework/components/force_joint_component.h"

namespace ingredients {
	void backpack(augs::entity_id e) {
		components::transform transform;
		components::container container;
		components::item item;
		components::force_joint force_joint;
		components::trigger trigger;
		
		inventory_slot slot_def;
		slot_def.space_available = 7;

		container.slots[slot_function::ITEM_DEPOSIT] = slot_def;

		item.dual_wield_accuracy_loss_multiplier = 1;
		item.dual_wield_accuracy_loss_percentage = 50;
		item.space_occupied_per_charge = 1;
		item.categories_for_slot_compatibility = item_category::SHOULDER_CONTAINER;

		// trigger.entity_to_be_notified = e;

		e->add(transform);
		e->add(container);
		e->add(item);
		e->add(trigger);
		e->add(force_joint);
		e->disable(force_joint);
	}
}