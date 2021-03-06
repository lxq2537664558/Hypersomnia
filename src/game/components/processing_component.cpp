#include "processing_component.h"
#include "game/systems_inferred/processing_lists_system.h"
#include "game/transcendental/entity_handle.h"
#include "game/transcendental/cosmos.h"
#include "game/transcendental/types_specification/all_component_includes.h"
#include "augs/misc/constant_size_vector.h"

typedef components::processing P;

P P::get_default(const const_entity_handle id) {
	augs::constant_size_vector<processing_subjects, static_cast<size_t>(processing_subjects::COUNT)> matching;

	if (id.has<components::animation>()) {
		matching.push_back(processing_subjects::WITH_ANIMATION);
	}
	if (id.has<components::behaviour_tree>()) {
		matching.push_back(processing_subjects::WITH_BEHAVIOUR_TREE);
	}
	if (id.has<components::car>()) {
		matching.push_back(processing_subjects::WITH_CAR);
	}
	if (id.has<components::crosshair>()) {
		matching.push_back(processing_subjects::WITH_CROSSHAIR);
	}
	if (id.has<components::missile>()) {
		matching.push_back(processing_subjects::WITH_DAMAGE);
	}
	if (id.has<components::driver>()) {
		matching.push_back(processing_subjects::WITH_DRIVER);
	}
	if (id.has<components::force_joint>()) {
		matching.push_back(processing_subjects::WITH_FORCE_JOINT);
	}
	if (id.has<components::gun>()) {
		matching.push_back(processing_subjects::WITH_GUN);
	}
	if (id.has<components::hand_fuse>()) {
		matching.push_back(processing_subjects::WITH_HAND_FUSE);
	}
	if (id.has<components::item_slot_transfers>()) {
		matching.push_back(processing_subjects::WITH_ITEM_SLOT_TRANSFERS);
	}
	if (id.has<components::interpolation>()) {
		matching.push_back(processing_subjects::WITH_INTERPOLATION);
	}
	if (id.has<components::melee>()) {
		matching.push_back(processing_subjects::WITH_MELEE);
	}
	if (id.has<components::movement>()) {
		matching.push_back(processing_subjects::WITH_MOVEMENT);
	}
	if (id.has<components::particles_existence>()) {
		matching.push_back(processing_subjects::WITH_PARTICLES_EXISTENCE);
	}
	if (id.has<components::sound_existence>()) {
		matching.push_back(processing_subjects::WITH_SOUND_EXISTENCE);
	}
	if (id.has<components::pathfinding>()) {
		matching.push_back(processing_subjects::WITH_PATHFINDING);
	}
	if (id.has<components::position_copying>()) {
		matching.push_back(processing_subjects::WITH_POSITION_COPYING);
	}
	if (id.has<components::rotation_copying>()) {
		matching.push_back(processing_subjects::WITH_ROTATION_COPYING);
	}
	if (id.has<components::sentience>()) {
		matching.push_back(processing_subjects::WITH_SENTIENCE);
	}
	if (id.has<components::trace>()) {
		matching.push_back(processing_subjects::WITH_TRACE);
	}
	if (id.has<components::light>()) {
		matching.push_back(processing_subjects::WITH_LIGHT);
	}
	if (id.get_flag(entity_flag::IS_PAST_CONTAGIOUS)) {
		matching.push_back(processing_subjects::WITH_ENABLED_PAST_CONTAGIOUS);
	}

	P result;

	for (const auto m : matching) {
		result.processing_subject_categories.set(m);
	}

	return result;
}

template<bool C>
bool basic_processing_synchronizer<C>::is_activated() const {
	return get_raw_component().activated;
}

template<bool C>
bool basic_processing_synchronizer<C>::is_in(const processing_subjects list) const {
	return get_raw_component().processing_subject_categories.test(list) && !get_raw_component().disabled_categories.test(list);
}

void component_synchronizer<false, P>::reinference() const {
	handle.get_cosmos().partial_reinference<processing_lists_system>(handle);
}

void component_synchronizer<false, P>::disable_in(const processing_subjects list) const {
	get_raw_component().disabled_categories.set(list, true);
	reinference();
}

void component_synchronizer<false, P>::enable_in(const processing_subjects list) const {
	get_raw_component().disabled_categories.set(list, false);
	reinference();
}

template<bool C>
P::flagset_type basic_processing_synchronizer<C>::get_disabled_categories() const {
	return get_raw_component().disabled_categories;
}

template<bool C>
P::flagset_type basic_processing_synchronizer<C>::get_basic_categories() const {
	return get_raw_component().processing_subject_categories;
}

void component_synchronizer<false, P>::set_disabled_categories(const P::flagset_type& categories) const {
	get_raw_component().disabled_categories = categories;
	reinference();
}

void component_synchronizer<false, P>::set_basic_categories(const P::flagset_type& categories) const {
	get_raw_component().processing_subject_categories = categories;
	reinference();
}

template class basic_processing_synchronizer<false>;
template class basic_processing_synchronizer<true>;