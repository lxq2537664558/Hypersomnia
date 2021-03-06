#include "game/transcendental/cosmos.h"
#include "game/transcendental/types_specification/all_component_includes.h"

#include "augs/templates/predicate_templates.h"
#include "augs/templates/get_index_type_for_size_of.h"

#include "generated/introspectors.h"
#include "game/assets/assets_manager.h"

#include "augs/misc/templated_readwrite.h"
#include "augs/templates/container_traits.h"
#include "game/components/pathfinding_component.h"

#include "augs/pad_bytes.h"
#include "augs/misc/custom_lua_representations.h"
#include "augs/misc/lua_readwrite.h"

namespace templates_detail {
	template <class T>
	struct identity {
		using type = T;
	};
}

struct tests_of_traits {
	static_assert(
		sizeof(entity_id) >= sizeof(entity_guid)
		&& alignof(entity_id) >= alignof(entity_guid),
		"With given memory layouts, entity_id<->entity_guid substitution will not be possible in delta encoding"
	);

	static_assert(is_component_fundamental_v<components::name>);

	static_assert(!is_padding_field_v<entity_id>);
	static_assert(is_padding_field_v<pad_bytes<4>>);
	static_assert(is_padding_field_v<pad_bytes<1>>);

	static_assert(!has_introspect_v<cosmos>, "Trait has failed");
	static_assert(has_introspect_v<cosmos_metadata>, "Trait has failed");
	static_assert(has_introspect_v<augs::constant_size_vector<int, 2>>, "Trait has failed");
	static_assert(has_introspect_v<zeroed_pod<unsigned int>>, "Trait has failed");
	static_assert(has_introspect_v<augs::delta>, "Trait has failed");
	static_assert(alignof(meter_instance_tuple) == 4, "Trait has failed");

	static_assert(std::is_same_v<std::tuple<int, double, float>, reverse_types_in_list_t<std::tuple<float, double, int>>>, "Trait has failed");
	static_assert(std::is_same_v<type_list<int, double, float>, reverse_types_in_list_t<type_list<float, double, int>>>, "Trait has failed");

	static_assert(sum_sizes_until_nth_v<0, std::tuple<int, double, float>> == 0, "Trait has failed");
	static_assert(sum_sizes_until_nth_v<1, std::tuple<int, double, float>> == 4, "Trait has failed");
	static_assert(sum_sizes_until_nth_v<2, std::tuple<int, double, float>> == 12, "Trait has failed");
	static_assert(sum_sizes_until_nth_v<3, std::tuple<int, double, float>> == 16, "Trait has failed");

	static_assert(sum_sizes_of_types_in_list_v<std::tuple<int, double, float>> == 16, "Trait has failed");
	static_assert(sum_sizes_of_types_in_list_v<std::tuple<int>> == 4, "Trait has failed");
	static_assert(sum_sizes_of_types_in_list_v<std::tuple<>> == 0, "Trait has failed");

	static_assert(count_occurences_in_v<int, int, double, float> == 1, "Trait has failed");
	static_assert(count_occurences_in_list_v<int, std::tuple<int, double, float>> == 1, "Trait has failed");
	static_assert(count_occurences_in_list_v<int, std::tuple<int, double, float, int>> == 2, "Trait has failed");

	static_assert(!is_constexpr_size_container_v<std::vector<int>>, "Trait has failed");
	static_assert(is_constexpr_size_container_v<std::array<int, 3>>, "Trait has failed");
	static_assert(is_constexpr_size_container_v<std::array<vec2, 3>>, "Trait has failed");
	static_assert(is_constexpr_size_container_v<decltype(pad_bytes<3>::pad)>, "Trait has failed");
	static_assert(!is_variable_size_container_v<decltype(pad_bytes<3>::pad)>, "Trait has failed");
	static_assert(is_variable_size_container_v<augs::enum_associative_array<intent_type, vec2>>, "Trait has failed");

	
	static_assert(is_unary_container_v<augs::constant_size_vector<vec2, 20>>, "Trait has failed");
	static_assert(is_variable_size_container_v<augs::constant_size_vector<vec2, 20>>, "Trait has failed");
	static_assert(augs::has_io_overloads_v<augs::stream, augs::constant_size_vector<vec2, 20>>, "Trait has failed");
	static_assert(augs::has_io_overloads_v<augs::stream, augs::enum_associative_array<intent_type, vec2>>, "Trait has failed");
	static_assert(augs::has_io_overloads_v<augs::stream, std::vector<int>>, "Trait has failed");
	static_assert(augs::has_io_overloads_v<augs::stream, std::vector<vec2>>, "Trait has failed");
	static_assert(augs::has_io_overloads_v<augs::stream, std::vector<cosmos>>, "Trait has failed");
	static_assert(augs::has_io_overloads_v<augs::stream, std::vector<pathfinding_session>>, "Trait has failed");

	static_assert(can_access_data_v<std::string>, "Trait has failed");
	static_assert(can_access_data_v<std::vector<int>>, "Trait has failed");
	static_assert(!can_access_data_v<std::set<int>>, "Trait has failed");
	static_assert(can_reserve_v<std::vector<int>>, "Trait has failed");
	static_assert(!can_reserve_v<std::map<int, int>>, "Trait has failed");

	static_assert(!has_introspect_v<unsigned>, "Trait has failed");
	static_assert(has_introspect_v<ltrbt<float>>, "Trait has failed");
	static_assert(has_introspect_v<ltrbt<int>>, "Trait has failed");

	static_assert(bind_types<std::is_same, const int>::type<const int>::value, "Trait has failed");

	static_assert(!bind_types_right<is_one_of, int, double, unsigned, signed>::type<float>::value, "Trait has failed");

	static_assert(does_asset_define_get_logical_meta<augs::enum_associative_array<assets::animation_id, animation>>::value, "Trait has failed");

	static_assert(
		std::is_same_v<
			typename make_container_of_logical_metas<augs::enum_associative_array<assets::animation_id, animation>>::type,
			augs::enum_associative_array<assets::animation_id, animation>
		>,
		"Trait has failed"
	);

	static_assert(
			std::is_same_v<
			filter_types_in_list_t<does_asset_define_get_logical_meta, std::tuple<augs::enum_associative_array<assets::animation_id, animation>>>,
			std::tuple<augs::enum_associative_array<assets::animation_id, animation>>
		>,
		"Trait has failed"
	);
	
	static_assert(std::is_same_v<filter_types_in_list<std::is_integral, type_list<double, int, float>>::indices, std::index_sequence<1>>, "Trait has failed");
	static_assert(std::is_same_v<filter_types_in_list<std::is_integral, type_list<double, int, float>>::type, std::tuple<int>>, "Trait has failed");
	static_assert(std::is_same_v<filter_types_in_list<std::is_integral, type_list<double, int, float>>::get_type<0>::type, int>, "Trait has failed");
	
	static_assert(is_one_of_list_v<unsigned, std::tuple<float, float, double, unsigned>>, "Trait has failed");
	static_assert(!is_one_of_v<int, float, double>, "Trait has failed");
	static_assert(is_one_of_v<unsigned, float, float, double, unsigned>, "Trait has failed");
	static_assert(is_one_of_v<cosmos, int, cosmos_metadata, cosmos>, "Trait has failed");

	static_assert(index_in_list_v<unsigned, std::tuple<float, float, double, unsigned>> == 3, "Trait has failed");
	static_assert(index_in_v<unsigned, float, float, double, unsigned> == 3, "Trait has failed");
	
	static_assert(std::is_same_v<unsigned, nth_type_in_t<0, unsigned, float, float>>, "Trait has failed");
	static_assert(std::is_same_v<double, nth_type_in_t<3, unsigned, float, float, double, unsigned>>, "Trait has failed");
	
	static_assert(
		std::is_same_v<
			filter_types_in_list<std::is_integral, type_list<int, double, float, unsigned>>::type, 
			std::tuple<int, unsigned>
		>, 
		"Trait has failed"
	);
	
	static_assert(
		std::is_same_v<
		filter_types_in_list<std::is_integral, type_list<int, double, float, unsigned>>::indices,
			std::index_sequence<0, 3>
		>, 
		"Trait has failed"
	);
	
	static_assert(
		!std::is_same_v<
		filter_types_in_list<std::is_floating_point, type_list<int, double, float, unsigned>>::type,
			std::tuple<int, unsigned>
		>, 
		"Trait has failed"
	);

	static_assert(
		std::is_same_v<type_list<int&, double&, float&>, transform_types_in_list_t<type_list<int, double, float>, std::add_lvalue_reference>>,
		"Trait has failed."
	);
	
	static_assert(
		std::is_same_v<type_list<int, double, float>, transform_types_in_list_t<type_list<const int&, double&&, float&>, std::decay>>,
		"Trait has failed."
	);
	
	static_assert(
		std::is_same_v<type_list<const int&, double&&, float&>, transform_types_in_list_t<type_list<const int&, double&&, float&>, templates_detail::identity>>,
		"Trait has failed."
	);

	struct A {
		char a;
	};

	struct B {
		char a[255];
	};

	struct C {
		char a[256];
	};

	struct D {
		char a[257];
	};

	struct E {
		char a[65536];
	};

	struct F {
		char a[65537];
	};

	static_assert(std::is_same_v<unsigned char, get_index_type_for_size_of_t<A>>, "Trait has failed");
	static_assert(std::is_same_v<unsigned char, get_index_type_for_size_of_t<B>>, "Trait has failed");
	static_assert(std::is_same_v<unsigned char, get_index_type_for_size_of_t<C>>, "Trait has failed");
	static_assert(std::is_same_v<unsigned short, get_index_type_for_size_of_t<D>>, "Trait has failed");
	static_assert(std::is_same_v<unsigned short, get_index_type_for_size_of_t<E>>, "Trait has failed");
	static_assert(std::is_same_v<unsigned int, get_index_type_for_size_of_t<F>>, "Trait has failed");

	static_assert(sizeof(cosmos) < 1000000, "Possible stack overflow due to cosmos on the stack");

	static_assert(is_introspective_leaf_v<launch_type>);
	static_assert(has_enum_to_string_v<launch_type>);
	static_assert(has_enum_to_string_v<launch_type>);
	static_assert(has_for_each_enum_v<launch_type>);
	static_assert(has_for_each_enum_v<input_recording_type>);
	static_assert(is_container_v<decltype(input_context::key_to_intent)>);

	static_assert(augs::has_custom_to_lua_value_v<rgba>);
	static_assert(augs::has_custom_to_lua_value_v<ImVec4>);
	static_assert(augs::has_custom_to_lua_value_v<std::wstring>);
	static_assert(augs::representable_as_lua_value_v<std::wstring>);
	static_assert(augs::representable_as_lua_value_v<const std::wstring*>);
	
	static_assert(!augs::has_io_overloads_v<augs::stream, put_all_components_into_t<augs::component_aggregate>>);
};

/*
constexpr auto assets_manager_size = sizeof(assets_manager);
constexpr auto tuple_of_assets_size = sizeof(tuple_of_all_assets);
constexpr auto tuple_of_logical_metas_of_assets_size = sizeof(tuple_of_all_logical_metas_of_assets);

sizeof(augs::enum_associative_array<assets::animation_id, animation>);
sizeof(augs::enum_associative_array<assets::game_image_id, game_image_baked>);
sizeof(augs::enum_associative_array<assets::font_id, game_font_baked>);
sizeof(augs::enum_associative_array<assets::particle_effect_id, particle_effect>);
sizeof(augs::enum_associative_array<assets::tile_layer_id, tile_layer>);
sizeof(augs::enum_associative_array<assets::physical_material_id, physical_material>);
sizeof(augs::enum_associative_array<assets::shader_id, augs::graphics::shader>);
sizeof(augs::enum_associative_array<assets::shader_program_id, augs::graphics::shader_program>);
sizeof(augs::enum_associative_array<assets::sound_buffer_id, augs::sound_buffer>);
sizeof(augs::enum_associative_array<assets::gl_texture_id, augs::graphics::texture>);
sizeof(particle_effect_logical_meta);

static_assert(
sizeof(tuple_of_all_logical_metas_of_assets) <= sizeof(tuple_of_all_assets),
"Metadatas should not be bigger than the objects themselves!"
);

static_assert(
sizeof(logical_meta_type) <= sizeof(typename T::mapped_type),
"Metadata should not be bigger than the object itself!"
);
*/