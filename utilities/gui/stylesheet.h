#pragma once
#include "stroke.h"
#include "rect.h"

namespace augs {
	namespace graphics {
		namespace gui {
			struct stylesheet {
				enum class appearance {
					released,
					hovered,
					pushed,
					unknown
				};

				/* how should rect look like depending on incoming event */
				static appearance map_event_to_appearance_type(rect::gui_event m);

				template <class T>
				struct attribute {
					bool active;
					T value;

					attribute(const T& value) : value(value), active(false) {}

					void set(const T& v) {
						value = v;
						active = true;
					}

					attribute& operator=(const T& v) { 
						set(v); 
						return *this; 
					}

					attribute& operator=(const attribute& attr) { 
						if(attr.active) { 
							value = attr.value;
							active = true; 
						} 
						return *this;
					}

					operator T() const {
						return value;
					}
				};

				struct style {
					attribute<rgba> color;
					attribute<assets::texture_id> background_image;
					attribute<solid_stroke> border;

					style();
					style(const attribute<rgba>& color, 
						const attribute<assets::texture_id>& background_image,
						const attribute<solid_stroke>& border);

					operator material() const;

				} released, hovered, pushed, focused;

				stylesheet(const style& released = style(), 
						   const style& hovered = style(), 
						   const style& pushed = style(),
						   const style& focused = style());

				appearance current_appearance;

				void update_appearance(rect::gui_event);
				style get_style() const;

			private:
				bool focus_flag;
			};
		}
	}
}