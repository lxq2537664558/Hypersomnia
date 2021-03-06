#include <fstream>

#include "cosmic_movie_director.h"

#include "augs/filesystem/file.h"
#include "augs/filesystem/directory.h"
#include "augs/misc/templated_readwrite.h"

#include "augs/templates/string_templates.h"

#include "generated/introspectors.h"

void cosmic_movie_director::save_recording_to_file(const std::string& filename) const {
	std::ofstream f(filename, std::ios::out | std::ios::binary);

	for (const auto& it : step_to_entropy) {
		augs::write(f, it.first);
		augs::write(f, it.second);
	}
}

guid_mapped_entropy cosmic_movie_director::get_entropy_for_step(const unsigned step) const {
	const auto it = step_to_entropy.find(step);

	if (it != step_to_entropy.end()) {
		return (*it).second;
	}

	return {};
}

bool cosmic_movie_director::is_recording_available() const {
	return step_to_entropy.size() > 0;
}

bool cosmic_movie_director::load_recording_from_file(const std::string& filename) {
	player_step_position = 0;
	step_to_entropy.clear();

	augs::read_map_until_eof(filename, step_to_entropy);

	return is_recording_available();
}