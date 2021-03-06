#include <sstream>

#include "neon_maps.h"
#include "augs/filesystem/file.h"
#include "augs/filesystem/directory.h"

#include "augs/ensure.h"
#include "augs/misc/streams.h"

#include "augs/image/image.h"
#include "augs/misc/lua_readwrite.h"
#include "generated/introspectors.h"

#define PIXEL_NONE rgba(0,0,0,0)

void make_neon(
	const neon_map_input& input,
	augs::image& source
);

std::vector<std::vector<double>> generate_gauss_kernel(
	const neon_map_input& input
);

std::vector<vec2u> hide_undesired_pixels(
	augs::image& original_image,
	const std::vector<rgba>& color_whitelist
);

void resize_image(
	augs::image& image_to_resize,
	const vec2u size
);

void cut_empty_edges(augs::image& source);

void regenerate_neon_map(
	const std::string& source_image_path,
	const std::string& output_image_path,
	const neon_map_input in,
	const bool force_regenerate
) {
	neon_map_stamp new_stamp;
	new_stamp.input = in;
	new_stamp.last_write_time_of_source = augs::last_write_time(source_image_path);

	const auto neon_map_path = output_image_path;
	const auto neon_map_stamp_path = augs::replace_extension(neon_map_path, ".stamp");

	augs::stream new_stamp_stream;
	augs::write(new_stamp_stream, new_stamp);

	bool should_regenerate = force_regenerate;

	if (!augs::file_exists(neon_map_path)) {
		should_regenerate = true;
	}
	else {
		if (!augs::file_exists(neon_map_stamp_path)) {
			should_regenerate = true;
		}
		else {
			augs::stream existent_stamp_stream;
			augs::get_file_contents_binary_into(neon_map_stamp_path, existent_stamp_stream);

			const bool are_stamps_identical = (new_stamp_stream == existent_stamp_stream);

			if (!are_stamps_identical) {
				should_regenerate = true;
			}
		}
	}

	if (should_regenerate) {
		LOG("Regenerating neon map for %x", source_image_path);

		auto source_image = augs::image(source_image_path);
		make_neon(new_stamp.input, source_image);

		source_image.save(neon_map_path);

		augs::create_directories(neon_map_stamp_path);
		augs::create_binary_file(neon_map_stamp_path, new_stamp_stream);
	}
}

void make_neon(
	const neon_map_input& input,
	augs::image& source
) {
	resize_image(source, vec2u(input.radius_towards_x_axis, input.radius_towards_y_axis));

	const auto pixel_list = hide_undesired_pixels(source, input.light_colors);

	const auto kernel = generate_gauss_kernel(input);

	std::vector<rgba> pixel_colors;

	for (const auto& pixel : pixel_list) {
		pixel_colors.push_back(source.pixel({ pixel.x, pixel.y }));
	}

	size_t i = 0;

	for (auto& pixel : pixel_list) {
		rgba center_pixel = pixel_colors[i];
		++i;

		const auto center_pixel_rgba = rgba{ center_pixel[2], center_pixel[1], center_pixel[0], center_pixel[3] };

		for (size_t y = 0; y < kernel.size(); ++y) {
			for (size_t x = 0; x < kernel[y].size(); ++x) {
				size_t current_index_y = pixel.y + y - input.radius_towards_y_axis / 2;

				if (current_index_y >= source.get_rows()) {
					continue;
				}

				size_t current_index_x = pixel.x + x - input.radius_towards_x_axis / 2;

				if (current_index_x >= source.get_columns()) {
					continue;
				}

				rgba& current_pixel = source.pixel({ current_index_x, current_index_y });
				auto current_pixel_rgba = rgba{ current_pixel[2], current_pixel[1], current_pixel[0], current_pixel[3] };
				size_t alpha = static_cast<size_t>(255 * kernel[y][x] * input.amplification);
				alpha = alpha > 255 ? 255 : alpha;

				if (current_pixel_rgba == PIXEL_NONE && alpha) {
					current_pixel[2] = center_pixel[2];
					current_pixel[1] = center_pixel[1];
					current_pixel[0] = center_pixel[0];
				}

				else if (current_pixel_rgba != center_pixel_rgba && alpha) {
					current_pixel[2] = static_cast<rgba_channel>((alpha * center_pixel[2] + current_pixel[3] * current_pixel[2]) / (alpha + current_pixel[3]));
					current_pixel[1] = static_cast<rgba_channel>((alpha * center_pixel[1] + current_pixel[3] * current_pixel[1]) / (alpha + current_pixel[3]));
					current_pixel[0] = static_cast<rgba_channel>((alpha * center_pixel[0] + current_pixel[3] * current_pixel[0]) / (alpha + current_pixel[3]));
				}

				if (alpha > current_pixel[3]) {
					current_pixel[3] = static_cast<rgba_channel>(alpha);
				}
			}
		}
	}

	for (size_t i = 0; i < pixel_list.size(); ++i) {
		source.pixel(pixel_list[i]) = pixel_colors[i];
	}

	cut_empty_edges(source);

	for (size_t y = 0; y < source.get_rows(); ++y) {
		for (size_t x = 0; x < source.get_columns(); ++x) {
			source.pixel({ x, y }).a = static_cast<rgba_channel>(source.pixel({ x, y }).a * input.alpha_multiplier);
		}
	}
}

std::vector<std::vector<double>> generate_gauss_kernel(const neon_map_input& input)
{
	const auto radius_towards_x_axis = input.radius_towards_x_axis;
	auto radius_towards_y_axis = input.radius_towards_y_axis;

	if (!input.radius_towards_y_axis) {
		radius_towards_y_axis = radius_towards_x_axis;
	}

	std::vector<std::vector<std::pair<int, int>>> index;

	auto max_index_x = radius_towards_x_axis / 2;
	auto max_index_y = radius_towards_y_axis / 2;

	index.resize(radius_towards_y_axis);

	for (auto& vector : index) {
		vector.resize(radius_towards_x_axis);
	}

	for (size_t y = 0; y < index.size(); ++y) {
		for (size_t x = 0; x < index[y].size(); ++x) {
			index[y][x] = std::make_pair(x - max_index_x, y - max_index_y);
		}
	}

	std::vector<std::vector<double>> result;
	result.resize(radius_towards_y_axis);

	for (auto& vector : result) {
		vector.resize(radius_towards_x_axis);
	}

	for (size_t y = 0; y < result.size(); ++y) {
		for (size_t x = 0; x < result[y].size(); ++x) {
			result[y][x] = exp(-1 * (pow(index[x][y].first, 2) + pow(index[x][y].second, 2)) / 2 / pow(input.standard_deviation, 2)) / PI<float> / 2 / pow(input.standard_deviation, 2);
		}
	}

	double sum = 0.f;

	for (const auto& vec_2d : result) {
		for (const auto& value : vec_2d) {
			sum += value;
		}
	}

	for (auto& vector : result) {
		for (auto& value : vector) {
			value /= sum;
		}
	}

	return result;
}


void resize_image(
	augs::image& image_to_resize,
	vec2u size
) {

	size.x = image_to_resize.get_columns() + size.x * 2;
	size.y = image_to_resize.get_rows() + size.y * 2;

	auto copy_mat = augs::image(size);

	auto offset_x = static_cast<int>(size.x - image_to_resize.get_columns()) / 2;

	if (offset_x < 0) {
		offset_x = 0;
	}

	auto offset_y = static_cast<int>(size.y - image_to_resize.get_rows()) / 2;

	if (offset_y < 0) {
		offset_y = 0;
	}

	for (size_t y = 0; y < image_to_resize.get_rows(); ++y) {
		for (size_t x = 0; x < image_to_resize.get_columns(); ++x)
		{
			copy_mat.pixel({ x + offset_x, y + offset_y }) = image_to_resize.pixel({ x, y });;
		}
	}

	image_to_resize = copy_mat;
}

std::vector<vec2u> hide_undesired_pixels(
	augs::image& original_image,
	const std::vector<rgba>& color_whitelist
) {
	std::vector<vec2u> result;

	for (size_t y = 0; y < original_image.get_rows(); ++y) {
		for (size_t x = 0; x < original_image.get_columns(); ++x)
		{
			auto& pixel = original_image.pixel({ x, y });
			auto found = find_if(color_whitelist.begin(), color_whitelist.end(), [pixel](const rgba& a) {
				return a == pixel;
			});

			if (found == color_whitelist.end())
			{
				pixel = PIXEL_NONE;
			}
			else {
				result.push_back(vec2u{ x, y });
			}
		}
	}

	return result;
}

void cut_empty_edges(augs::image& source) {
	vec2u output_size = source.get_size();
	vec2u offset = { 0,0 };
	for (size_t y = 0; y < source.get_rows() / 2; ++y) {
		bool pixel_found = false;
		for (size_t x = 0; x < source.get_columns(); ++x) {
			if (source.pixel({ x, y }) != PIXEL_NONE || source.pixel({ x, source.get_rows() - y - 1 }) != PIXEL_NONE) {
				pixel_found = true;
				break;
			}
		}
		if (!pixel_found) {
			output_size.y -= 2;
			++offset.y;
		}
		else
			break;
	}

	for (size_t x = 0; x < source.get_columns() / 2; ++x) {
		bool pixel_found = false;
		for (size_t y = 0; y < source.get_rows(); ++y) {
			if (source.pixel({ x, y }) != PIXEL_NONE || source.pixel({ source.get_columns() - x - 1, y }) != PIXEL_NONE) {
				pixel_found = true;
				break;
			}
		}
		if (!pixel_found) {
			output_size.x -= 2;
			++offset.x;
		}
		else
			break;
	}

	if (offset == vec2u(0, 0) || output_size.x == 0 || output_size.y == 0) {
		return;
	}

	auto copy = augs::image(output_size);

	for (size_t x = 0; x < copy.get_columns(); ++x) {
		for (size_t y = 0; y < copy.get_rows(); ++y) {
			copy.pixel({ x,y }) = source.pixel({ x + offset.x, y + offset.y });
		}
	}

	source = copy;
}