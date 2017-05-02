#pragma once
#include "setup_base.h"

#include "game/transcendental/cosmos.h"

#include "game/transcendental/simulation_receiver.h"
#include "game/view/viewing_session.h"
#include "augs/misc/debug_entropy_player.h"
#include "game/scene_builders/networked_testbed.h"

class client_setup : public setup_base {
public:
	bool predict_entropy = true;

	all_logical_metas_of_assets metas_of_assets;

	cosmos hypersomnia = cosmos(3000);
	cosmos initial_hypersomnia = cosmos(3000);

	cosmos hypersomnia_last_snapshot = cosmos(3000);
	cosmos extrapolated_hypersomnia = cosmos(3000);

	cosmic_entropy total_collected_entropy;
	augs::fixed_delta_timer timer = augs::fixed_delta_timer(5);
	scene_builders::networked_testbed_client scene;

	bool last_stepped_was_extrapolated = false;
	bool complete_state_received = false;

	bool detailed_step_log = false;

	augs::network::client client;
	simulation_receiver receiver;

	void process(
		const config_lua_table& cfg, 
		game_window&,
		viewing_session&
	);

	void init(
		const config_lua_table& cfg, 
		game_window&, 
		viewing_session&,
		const std::string recording_filename = "recorded.inputs",
		const bool use_alternative_port = false
	);

	void process_once(
		const config_lua_table& cfg, 
		game_window&,
		viewing_session&,
		const augs::machine_entropy::local_type& precollected, 
		const bool swap_buffers = true
	);
};