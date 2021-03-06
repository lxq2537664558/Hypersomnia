#pragma once

/* Why it's global:
	1. Debug drawing is performed in many places in the logic, even in some very modular ones (force application). 
	If we stored it somehow in logic_step, we would need to pass step information to these modular functionalities as well,
	even though only debug drawing settings are needed there, and possibly not at all needed.
	This would be ugly.

	2. Access to a global variable is faster which is important in the logic,
	especially given that might not want to use the debug drawing at all.

	3. We 'could' store it in augs::renderer, to which anyway global access is possible,
	but then we couple very game-specific flags with a structure that is just meant to be pushed triangles and lines into.
	Anyway, augs::renderer does not use that information at all.

	4. We couldn't care less. 
*/

struct debug_drawing_settings {
	// GEN INTROSPECTOR struct debug_drawing_settings
	bool enabled = false;

	bool draw_colinearization = false;
	bool draw_forces = false;
	bool draw_friction_field_collisions_of_entering = false;
	bool draw_explosion_forces = false;

	bool draw_triangle_edges = false;
	bool draw_cast_rays = false;
	bool draw_discontinuities = false;
	bool draw_visible_walls = false;

	bool draw_memorised_walls = false;
	bool draw_undiscovered_locations = false;
	// END GEN INTROSPECTOR
};

extern debug_drawing_settings DEBUG_DRAWING;