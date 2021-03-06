#pragma once
#include <unordered_map>
#include "augs/templates/introspect.h"
#include "augs/templates/container_templates.h"

namespace augs {
	template <class Enum>
	struct global_lookup {
		std::unordered_map<std::string, Enum> enums;

		global_lookup() {
			if constexpr(has_for_each_enum_v<Enum>) {
				for_each_enum<Enum>([this](const Enum e) {
					enums[enum_to_string(e)] = e;
				});
			}
			else {
				for (std::size_t i = 0; i < std::size_t(Enum::COUNT); ++i) {
					const Enum e = Enum(i);
					enums[enum_to_string(e)] = e;
				}
			}
		}
	};

	template <class Enum>
	global_lookup<Enum> string_to_enum_lookup;

	template <class Enum>
	Enum string_to_enum(const std::string& label) {
		return string_to_enum_lookup<Enum>.enums.at(label);
	}

	template <class Enum>
	Enum string_to_enum_or(const std::string& label, const Enum or = Enum::INVALID) {
		return found_or(string_to_enum_lookup<Enum>.enums, label, or);
	}
}