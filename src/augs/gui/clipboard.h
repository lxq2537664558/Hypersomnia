#pragma once
#include "formatted_text.h"

namespace augs {
	namespace gui {
		class clipboard {
			bool
				/* own_copy indicates whether the clipboard_change event comes from manual "copy_clipboard" or from external source */
				own_copy = false,
				own_clip = false;

		public:
			bool fetch_clipboard = true;

			text::formatted_string contents;

			void change_clipboard();
			void copy_clipboard(const text::formatted_string&);

			bool is_clipboard_own() const;
		};

		void paste_clipboard_formatted(text::formatted_string& out, text::formatted_char = text::formatted_char());
	}
}
