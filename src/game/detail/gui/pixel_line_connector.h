#pragma once
#include "augs/math/rects.h"
#include "augs/misc/constant_size_vector.h"
#include "augs/gui/gui_traversal_structs.h"
#include <array>

augs::constant_size_vector<std::array<vec2i, 2>, 2> get_connecting_pixel_lines(
	const ltrb& origin, 
	const ltrb& target
);

void draw_pixel_line_connector(
	const ltrb& origin, 
	const ltrb& target,
	const augs::gui::draw_info,
	const rgba col
);
