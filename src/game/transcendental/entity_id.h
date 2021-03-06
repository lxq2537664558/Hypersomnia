#pragma once
#include <type_traits>
#include "augs/misc/pooled_object_id.h"
#include "game/transcendental/types_specification/all_components_declaration.h"
#include "game/transcendental/entity_id_declaration.h"

namespace augs {
	template <class...>
	class component_aggregate;
}

struct entity_guid {
	typedef unsigned guid_value_type;
	// GEN INTROSPECTOR struct entity_guid
	guid_value_type value = 0u;
	// END GEN INTROSPECTOR

	entity_guid(const guid_value_type b = 0u) : value(b) {}
	
	entity_guid& operator=(const guid_value_type b) {
		value = b;
		return *this;
	}

	operator guid_value_type() const {
		return value;
	}
};

struct unversioned_entity_id : public augs::unversioned_id<put_all_components_into_t<augs::component_aggregate>> {
	typedef augs::unversioned_id<typename put_all_components_into<augs::component_aggregate>::type> base;

	unversioned_entity_id(const base b = base()) : base(b) {}
};

struct entity_id : public augs::pooled_object_id<put_all_components_into_t<augs::component_aggregate>> {
	// GEN INTROSPECTOR struct entity_id
	// INTROSPECT BASE augs::pooled_object_id<put_all_components_into_t<augs::component_aggregate>>
	// END GEN INTROSPECTOR

	typedef augs::pooled_object_id<put_all_components_into_t<augs::component_aggregate>> base;

	entity_id(const base b = base()) : base(b) {}

	operator unversioned_entity_id() const {
		return static_cast<unversioned_entity_id::base>(*static_cast<const base*>(this));
	}
};

struct child_entity_id : entity_id {
	// GEN INTROSPECTOR struct child_entity_id
	// INTROSPECT BASE entity_id
	// END GEN INTROSPECTOR

	typedef entity_id base;

	child_entity_id(const base b = base()) : base(b) {}

	using base::operator unversioned_entity_id;
};

namespace std {
	template <>
	struct hash<entity_guid> {
		std::size_t operator()(const entity_guid v) const {
			return hash<entity_guid::guid_value_type>()(v.value);
		}
	};

	template <>
	struct hash<entity_id> {
		std::size_t operator()(const entity_id v) const {
			return hash<entity_id::base>()(v);
		}
	};

	template <>
	struct hash<unversioned_entity_id> {
		std::size_t operator()(const unversioned_entity_id v) const {
			return hash<unversioned_entity_id::base>()(v);
		}
	};
}
