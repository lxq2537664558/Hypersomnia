#pragma once
#include <vector>
#include "3rdparty/rectpack2D/src/pack.h"

#include "augs/image/image.h"
#include "texture_atlas_entry.h"

namespace augs {
	class renderer;
	class texture_with_image;

	class texture_atlas {
		unsigned id = 0;
		bool mipmaps = false, lin = false, rep = true, built = false;
		std::vector<bin> bins;

		friend class renderer;

	public:
		~texture_atlas();

		std::vector<texture_with_image*> textures;

		texture_atlas_entry atlas_texture;
		image img;

		bool pack(), // max texture size 
			pack(int max_size);

		void create_image(const bool destroy_images);
		void build(bool mipmaps = false, bool linear = false, image* raw_texture = 0), nearest(), linear(), clamp(), repeat();

		void default_build();

		bool is_mipmapped() const;
		void destroy();
	};
}