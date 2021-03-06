#pragma once
#include "augs/templates/transform_types.h"
#include "augs/templates/instance_type.h"
#include "augs/templates/type_in_list_id.h"

#include "game/detail/perks/haste_perk.h"
#include "game/detail/perks/electric_shield_perk.h"

template<template <typename...> class List>
struct put_all_perks_into {
	using type = List<
		haste_perk,
		electric_shield_perk
	>;
};

template <template <typename...> class List>
using put_all_perks_into_t = typename put_all_perks_into<List>::type;

template <template <typename...> class List>
using put_all_perk_instances_into_t = transform_types_in_list_t<
	put_all_perks_into_t<List>,
	make_instance
>;

using perk_instance_tuple = put_all_perk_instances_into_t<augs::trivially_copyable_tuple>;
using perk_id = type_in_list_id<perk_instance_tuple>;