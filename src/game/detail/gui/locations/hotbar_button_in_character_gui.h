#pragma once

class hotbar_button;

struct hotbar_button_in_character_gui {
public:
	typedef hotbar_button dereferenced_type;

	int index = -1;

	bool operator==(const hotbar_button_in_character_gui b) const {
		return index == b.index;
	}

	template <class C>
	bool alive(const C context) const {
		return index >= 0 && index < static_cast<int>(context.get_character_gui().hotbar_buttons.size());
	}

	template <class C>
	decltype(auto) dereference(const C context) const {
		return &context.get_character_gui().hotbar_buttons.at(index);
	}
};


namespace std {
	template <>
	struct hash<hotbar_button_in_character_gui> {
		size_t operator()(const hotbar_button_in_character_gui& k) const {
			return std::hash<int>()(k.index);
		}
	};
}