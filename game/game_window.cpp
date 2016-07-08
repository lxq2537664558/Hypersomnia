#include "game_window.h"
#include "game/bindings/bind_game_and_augs.h"

game_window::game_window() {
	bind_game_and_augs(lua);
}

void game_window::call_window_script(std::string filename) {
	lua.global_ptr("global_gl_window", &window);

	try {
		if (!lua.dofile(filename))
			lua.debug_response();
	}
	catch (char* e) {
		LOG("Exception thrown! %x", e);
		lua.debug_response();
	}
	catch (...) {
		LOG("Exception thrown!");
		lua.debug_response();
	}

	window.gl.initialize();
}

augs::machine_entropy game_window::collect_entropy() {
	augs::machine_entropy result;
	
	result.local = window.poll_events();

	if (clear_window_inputs_once) {
		result.local.clear();
		clear_window_inputs_once = false;
	}
	
	return result;
}