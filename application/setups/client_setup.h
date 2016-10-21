#pragma once
#include "setup_base.h"

#include "game/transcendental/cosmos.h"

#include "game/transcendental/simulation_receiver.h"
#include "game/transcendental/viewing_session.h"
#include "game/transcendental/step_and_entropy_unpacker.h"
#include "game/scene_managers/networked_testbed.h"

class client_setup : public setup_base {
public:
	bool predict_entropy = true;

	cosmos hypersomnia = cosmos(3000);
	cosmos initial_hypersomnia = cosmos(3000);

	cosmos hypersomnia_last_snapshot = cosmos(3000);
	cosmos extrapolated_hypersomnia = cosmos(3000);

	viewing_session session;

	step_and_entropy_unpacker input_unpacker;
	scene_managers::networked_testbed_client scene;

	bool last_stepped_was_extrapolated = false;
	bool complete_state_received = false;

	bool detailed_step_log = false;

	augs::network::client client;
	simulation_receiver receiver;

	void process(game_window&);

	void init(game_window&, const std::string recording_filename = "recorded.inputs", const bool use_alternative_port = false);
	void process_once(game_window&, const augs::machine_entropy::local_type& precollected, const bool swap_buffers = true);
};