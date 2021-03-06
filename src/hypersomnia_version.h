#include <string>
#include <vector>

/*
	This will always be written to the beginning of the savefiles so that we know their version for sure.
	The layout of this structure should stay the same for long time to come.
*/

struct hypersomnia_version {
	hypersomnia_version();
	// GEN INTROSPECTOR struct hypersomnia_version
	unsigned commit_number;
	std::string commit_message;
	std::string commit_date;
	std::string commit_hash;
	std::vector<std::string> working_tree_changes;
	// END GEN INTROSPECTOR
};