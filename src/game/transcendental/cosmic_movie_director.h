#pragma once
#include <map>
#include "cosmic_entropy.h"

class cosmic_movie_director {
public:
	unsigned player_step_position = 0u;

	std::map<unsigned, guid_mapped_entropy> step_to_entropy;

	guid_mapped_entropy get_entropy_for_step(const unsigned) const;

	bool is_recording_available() const;
	bool load_recording_from_file(const std::string&);
	void save_recording_to_file(const std::string&) const;
};