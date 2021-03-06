#pragma once
#include <memory>
#include "augs/misc/enum_boolset.h"

struct FT_LibraryRec_;
typedef struct FT_LibraryRec_  *FT_Library;

namespace augs {
	struct global_libraries {
		enum class library {
			FREETYPE,
			ENET,

			COUNT
		};

		using library_flagset = augs::enum_boolset<library>;

		library_flagset initialized;
		static std::unique_ptr<FT_Library> freetype_library;

		global_libraries(const library_flagset = { library::FREETYPE, library::ENET });
		void init		(const library_flagset = { library::FREETYPE, library::ENET });
		void deinit		(const library_flagset = { library::FREETYPE, library::ENET });
		~global_libraries();
	};
};