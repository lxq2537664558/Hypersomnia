#include "entity_system/world.h"
#include "game/components/gun_component.h"
#include "game/components/item_component.h"
#include "game/components/physics_definition_component.h"
#include "game/components/damage_component.h"
#include "game/components/sprite_component.h"
#include "game/components/name_component.h"
#include "game/components/trace_component.h"
#include "game/components/particle_effect_response_component.h"

#include "game/globals/filters.h"

#include "entity_system/world.h"

#include "game/messages/item_slot_transfer_request.h"

#include "ingredients.h"
#include "game/detail/inventory_utils.h"

namespace ingredients {
	void default_gun_container(augs::entity_id e) {
		auto& item = make_item(e);
		auto& container = *e += components::container();
		item.space_occupied_per_charge = to_space_units("3.5");

		{
			inventory_slot slot_def;
			slot_def.is_physical_attachment_slot = true;
			slot_def.always_allow_exactly_one_item = true;
			slot_def.for_categorized_items_only = true;
			slot_def.category_allowed = item_category::MAGAZINE;
			slot_def.attachment_sticking_mode = augs::rects::sticking::TOP;
			slot_def.attachment_offset.pos.set(10, 5);
			slot_def.attachment_offset.rotation = -90;

			container.slots[slot_function::GUN_DETACHABLE_MAGAZINE] = slot_def;
		}

		{
			inventory_slot slot_def;
			slot_def.is_physical_attachment_slot = false;
			slot_def.always_allow_exactly_one_item = true;
			slot_def.for_categorized_items_only = true;
			slot_def.category_allowed = item_category::SHOT_CHARGE;
			slot_def.space_available = to_space_units("0.01");

			container.slots[slot_function::GUN_CHAMBER] = slot_def;
		}

		{
			inventory_slot slot_def;
			slot_def.is_physical_attachment_slot = true;
			slot_def.always_allow_exactly_one_item = true;
			slot_def.for_categorized_items_only = true;
			slot_def.category_allowed = item_category::BARREL_ATTACHMENT;
			slot_def.attachment_sticking_mode = augs::rects::sticking::RIGHT;

			container.slots[slot_function::GUN_BARREL] = slot_def;
		}
	}
}

namespace prefabs {
	augs::entity_id create_sample_magazine(augs::world& world, vec2 pos, std::string space, augs::entity_id charge_inside) {
		auto sample_magazine = world.create_entity("sample_magazine");
		name_entity(sample_magazine, entity_name::MAGAZINE);

		{
			ingredients::sprite(sample_magazine, pos, assets::texture_id::SAMPLE_MAGAZINE, augs::white, render_layer::DROPPED_ITEM);
			ingredients::crate_physics(sample_magazine);

			auto& item = ingredients::make_item(sample_magazine);
			auto& container = *sample_magazine += components::container();
			
			item.categories_for_slot_compatibility = item_category::MAGAZINE;
			item.space_occupied_per_charge = to_space_units("0.5");

			inventory_slot charge_deposit_def;
			charge_deposit_def.for_categorized_items_only = true;
			charge_deposit_def.category_allowed = item_category::SHOT_CHARGE;
			charge_deposit_def.space_available = to_space_units(space);

			container.slots[slot_function::ITEM_DEPOSIT] = charge_deposit_def;
		}

		if (charge_inside.dead()) {
			charge_inside = create_cyan_charge(world, vec2(0, 0), 30);
		}

		messages::item_slot_transfer_request load_charge;
		load_charge.item = charge_inside;
		load_charge.target_slot = sample_magazine[slot_function::ITEM_DEPOSIT];

		world.post_message(load_charge);

		return sample_magazine;
	}

	augs::entity_id create_sample_suppressor(augs::world& world, vec2 pos) {
		auto sample_suppressor = world.create_entity("sample_suppressor");
		name_entity(sample_suppressor, entity_name::SUPPRESSOR);

		ingredients::sprite(sample_suppressor, pos, assets::texture_id::SAMPLE_SUPPRESSOR, augs::white, render_layer::DROPPED_ITEM);
		ingredients::crate_physics(sample_suppressor);

		auto& item = ingredients::make_item(sample_suppressor);

		item.categories_for_slot_compatibility = item_category::BARREL_ATTACHMENT;
		item.space_occupied_per_charge = to_space_units("0.2");

		return sample_suppressor;
	}

	augs::entity_id create_pink_charge(augs::world& world, vec2 pos, int charges) {
		auto pink_charge = world.create_entity("pink_charge");
		auto round_definition = world.create_definition_entity("round_definition");
		auto shell_definition = world.create_definition_entity("shell_definition");
		name_entity(pink_charge, entity_name::PINK_CHARGE);

		{
			ingredients::sprite(pink_charge, pos, assets::texture_id::PINK_CHARGE, augs::white, render_layer::DROPPED_ITEM);
			ingredients::crate_physics(pink_charge);

			auto& item = ingredients::make_item(pink_charge);
			item.space_occupied_per_charge = to_space_units("0.01");
			item.categories_for_slot_compatibility = item_category::SHOT_CHARGE;
			item.charges = charges;
			item.stackable = true;
		}

		{
			auto& s = ingredients::sprite(round_definition, pos, assets::texture_id::ROUND_TRACE, augs::pink, render_layer::FLYING_BULLETS);
			s.size *= vec2(2, 0.5);
			auto& def = ingredients::bullet_round_physics(round_definition);
			
			auto& damage = *round_definition += components::damage();
			damage.impulse_upon_hit = 1000.f;
			damage.effects_color = pink;

			auto& response = *round_definition += components::particle_effect_response();
			response.response = assets::particle_effect_response_id::ELECTRIC_CHARGE_RESPONSE;

			auto& trace = *round_definition += components::trace();
			trace.max_multiplier_x = std::make_pair(0.0f, 1.2f);
			trace.max_multiplier_y = std::make_pair(0.f, 0.f);
			trace.lengthening_duration_ms = std::make_pair(200.f, 250.f);
		}

		{
			ingredients::sprite(shell_definition, pos, assets::texture_id::PINK_SHELL, augs::white, render_layer::FLYING_BULLETS);
			auto& def = ingredients::crate_physics(shell_definition);
			def.fixtures[0].restitution = 1.4;
			def.fixtures[0].density = 0.001f;
			def.fixtures[0].filter = filters::shell();
		}

		pink_charge->map_sub_definition(sub_definition_name::BULLET_ROUND, round_definition);
		pink_charge->map_sub_definition(sub_definition_name::BULLET_SHELL, shell_definition);

		return pink_charge;
	}

	augs::entity_id create_cyan_charge(augs::world& world, vec2 pos, int charges) {
		auto cyan_charge = world.create_entity("cyan_charge");
		auto round_definition = world.create_definition_entity("round_definition");
		auto shell_definition = world.create_definition_entity("shell_definition");
		name_entity(cyan_charge, entity_name::CYAN_CHARGE);

		{
			ingredients::sprite(cyan_charge, pos, assets::texture_id::CYAN_CHARGE, augs::white, render_layer::DROPPED_ITEM);
			ingredients::crate_physics(cyan_charge);

			auto& item = ingredients::make_item(cyan_charge);
			item.space_occupied_per_charge = to_space_units("0.007");
			item.categories_for_slot_compatibility = item_category::SHOT_CHARGE;
			item.charges = charges;
			item.stackable = true;
		}

		{
			auto& s = ingredients::sprite(round_definition, pos, assets::texture_id::ROUND_TRACE, augs::cyan, render_layer::FLYING_BULLETS);
			s.size *= vec2(2, 0.5);
			auto& def = ingredients::bullet_round_physics(round_definition);

			auto& response = *round_definition += components::particle_effect_response();
			response.response = assets::particle_effect_response_id::ELECTRIC_CHARGE_RESPONSE;

			auto& damage = *round_definition += components::damage();
			damage.effects_color = cyan;
			auto& trace = *round_definition += components::trace();
			trace.max_multiplier_x = std::make_pair(0.0f, 1.2f);
			trace.max_multiplier_y = std::make_pair(0.f, 0.f);
			trace.lengthening_duration_ms = std::make_pair(200.f, 250.f);
		}

		{
			ingredients::sprite(shell_definition, pos, assets::texture_id::CYAN_SHELL, augs::white, render_layer::FLYING_BULLETS);
			auto& def = ingredients::crate_physics(shell_definition);
			def.fixtures[0].restitution = 1.4;
			def.fixtures[0].density = 0.001f;
			def.fixtures[0].filter = filters::shell();
		}

		cyan_charge->map_sub_definition(sub_definition_name::BULLET_ROUND, round_definition);
		cyan_charge->map_sub_definition(sub_definition_name::BULLET_SHELL, shell_definition);

		return cyan_charge;
	}

	augs::entity_id create_sample_rifle(augs::world& world, vec2 pos) {
		auto sample_rifle = world.create_entity("sample_rifle");
		name_entity(sample_rifle, entity_name::ASSAULT_RIFLE);

		auto& sprite = ingredients::sprite(sample_rifle, pos, assets::texture_id::ASSAULT_RIFLE, augs::white, render_layer::DROPPED_ITEM);
		auto& def = ingredients::crate_physics(sample_rifle);
		ingredients::default_gun_container(sample_rifle);

		auto& gun = *sample_rifle += components::gun();

		gun.action_mode = components::gun::AUTOMATIC;
		gun.muzzle_velocity = std::make_pair(4000, 4000);
		gun.timeout_between_shots.set(100);
		gun.bullet_spawn_offset.set(sprite.size.x/2, 0);
		gun.camera_shake_radius = 5.f;
		gun.camera_shake_spread_degrees = 45.f;

		gun.shell_spawn_offset.pos.set(0, 10);
		gun.shell_spawn_offset.rotation = 45;
		gun.shell_angular_velocity = std::make_pair(2.f, 14.f);
		gun.shell_spread_degrees = 20.f;
		gun.shell_velocity = std::make_pair(300, 1700);
		gun.damage_multiplier = 1.f;

		gun.recoil.repeat_last_n_offsets = 20;

		gun.recoil.offsets = {
			{ vec2().set_from_degrees(0) },
			{ vec2().set_from_degrees(6) },
			{ vec2().set_from_degrees(-11) },
			{ vec2().set_from_degrees(7) },
			{ vec2().set_from_degrees(14) },
			{ vec2().set_from_degrees(-18) },
			{ vec2().set_from_degrees(24) },
			{ vec2().set_from_degrees(-33) },
			{ vec2().set_from_degrees(-4) },
			{ vec2().set_from_degrees(21) },
			{ vec2().set_from_degrees(0) },
			{ vec2().set_from_degrees(53) },
			{ vec2().set_from_degrees(10) },
			{ vec2().set_from_degrees(-30) },
			{ vec2().set_from_degrees(-60) },
			{ vec2().set_from_degrees(-70) },
			{ vec2().set_from_degrees(-80) },
			{ vec2().set_from_degrees(-20) },
			{ vec2().set_from_degrees(50) },
			{ vec2().set_from_degrees(80) },
			{ vec2().set_from_degrees(120) },
			{ vec2().set_from_degrees(60) },
			{ vec2().set_from_degrees(20) },
			{ vec2().set_from_degrees(40) },
			{ vec2().set_from_degrees(20) },
			{ vec2().set_from_degrees(-40) },
		};
		
		gun.recoil.scale = 30.0;

		return sample_rifle;
	}

	augs::entity_id create_submachine(augs::world& world, vec2 pos) {
		auto weapon = world.create_entity("submachine");
		name_entity(weapon, entity_name::SUBMACHINE);

		auto& sprite = ingredients::sprite(weapon, pos, assets::texture_id::SUBMACHINE, augs::white, render_layer::DROPPED_ITEM);
		auto& def = ingredients::crate_physics(weapon);
		ingredients::default_gun_container(weapon);

		auto& gun = *weapon += components::gun();

		gun.action_mode = components::gun::AUTOMATIC;
		gun.muzzle_velocity = std::make_pair(3000, 3000);
		gun.timeout_between_shots.set(50);
		gun.bullet_spawn_offset.set(sprite.size.x/2, 0);
		gun.camera_shake_radius = 5.f;
		gun.camera_shake_spread_degrees = 45.f;

		gun.shell_spawn_offset.pos.set(0, 10);
		gun.shell_spawn_offset.rotation = 45;
		gun.shell_angular_velocity = std::make_pair(2.f, 14.f);
		gun.shell_spread_degrees = 20.f;
		gun.shell_velocity = std::make_pair(300, 1700);
		gun.damage_multiplier = 1.f;
		
		gun.recoil.repeat_last_n_offsets = 20;

		gun.recoil.offsets = {
			{ vec2().set_from_degrees(0) },
			{ vec2().set_from_degrees(6) },
			{ vec2().set_from_degrees(-11) },
			{ vec2().set_from_degrees(7) },
			{ vec2().set_from_degrees(14) },
			{ vec2().set_from_degrees(-18) },
			{ vec2().set_from_degrees(24) },
			{ vec2().set_from_degrees(-33) },
			{ vec2().set_from_degrees(-4) },
			{ vec2().set_from_degrees(21) },
			{ vec2().set_from_degrees(0) },
			{ vec2().set_from_degrees(53) },
			{ vec2().set_from_degrees(10) },
			{ vec2().set_from_degrees(-30) },
			{ vec2().set_from_degrees(-60) },
			{ vec2().set_from_degrees(-70) },
			{ vec2().set_from_degrees(-80) },
			{ vec2().set_from_degrees(-20) },
			{ vec2().set_from_degrees(50) },
			{ vec2().set_from_degrees(80) },
			{ vec2().set_from_degrees(120) },
			{ vec2().set_from_degrees(60) },
			{ vec2().set_from_degrees(20) },
			{ vec2().set_from_degrees(40) },
			{ vec2().set_from_degrees(20) },
			{ vec2().set_from_degrees(-40) },
		};

		gun.recoil.scale = 30.0;

		return weapon;
	}

	augs::entity_id create_pistol(augs::world& world, vec2 pos) {
		auto weapon = world.create_entity("pistol");
		name_entity(weapon, entity_name::PISTOL);

		auto& sprite = ingredients::sprite(weapon, pos, assets::texture_id::PISTOL, augs::white, render_layer::DROPPED_ITEM);
		auto& def = ingredients::crate_physics(weapon);
		ingredients::default_gun_container(weapon);

		auto& gun = *weapon += components::gun();

		gun.action_mode = components::gun::SEMI_AUTOMATIC;
		gun.muzzle_velocity = std::make_pair(2500, 2500);
		gun.timeout_between_shots.set(150);
		gun.bullet_spawn_offset.set(sprite.size.x / 2, 0);
		gun.camera_shake_radius = 5.f;
		gun.camera_shake_spread_degrees = 45.f;

		gun.shell_spawn_offset.pos.set(0, 10);
		gun.shell_spawn_offset.rotation = 45;
		gun.shell_angular_velocity = std::make_pair(2.f, 14.f);
		gun.shell_spread_degrees = 20.f;
		gun.shell_velocity = std::make_pair(300, 1700);
		gun.damage_multiplier = 1.f;
		
		gun.recoil.repeat_last_n_offsets = 20;

		gun.recoil.offsets = {
			{ vec2().set_from_degrees(0) },
			{ vec2().set_from_degrees(6) },
			{ vec2().set_from_degrees(-11) },
			{ vec2().set_from_degrees(7) },
			{ vec2().set_from_degrees(14) },
			{ vec2().set_from_degrees(-18) },
			{ vec2().set_from_degrees(24) },
			{ vec2().set_from_degrees(-33) },
			{ vec2().set_from_degrees(-4) },
			{ vec2().set_from_degrees(21) },
			{ vec2().set_from_degrees(0) },
			{ vec2().set_from_degrees(53) },
			{ vec2().set_from_degrees(10) },
			{ vec2().set_from_degrees(-30) },
			{ vec2().set_from_degrees(-60) },
			{ vec2().set_from_degrees(-70) },
			{ vec2().set_from_degrees(-80) },
			{ vec2().set_from_degrees(-20) },
			{ vec2().set_from_degrees(50) },
			{ vec2().set_from_degrees(80) },
			{ vec2().set_from_degrees(120) },
			{ vec2().set_from_degrees(60) },
			{ vec2().set_from_degrees(20) },
			{ vec2().set_from_degrees(40) },
			{ vec2().set_from_degrees(20) },
			{ vec2().set_from_degrees(-40) },
		};


		gun.recoil.scale = 30.0;

		return weapon;
	}
}