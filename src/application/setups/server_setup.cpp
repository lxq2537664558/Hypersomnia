#include <thread>
#include "game/transcendental/cosmos.h"
#include "augs/misc/templated_readwrite.h"
#include "augs/global_libraries.h"
#include "augs/window_framework/window.h"

#include "game/assets/assets_manager.h"

#include "game/hardcoded_content/all_hardcoded_content.h"

#include "game/transcendental/types_specification/all_component_includes.h"
#include "game/view/viewing_session.h"
#include "game/transcendental/simulation_broadcast.h"
#include "game/transcendental/cosmic_delta.h"

#include "augs/misc/debug_entropy_player.h"

#include "augs/filesystem/file.h"

#include "server_setup.h"
#include "game/transcendental/network_commands.h"

#include "augs/network/reliable_channel.h"

#include "augs/misc/randomization.h"

#include "application/web_daemon/session_report.h"

#include "augs/templates/string_templates.h"
#include "augs/templates/container_templates.h"

#include "augs/templates/container_templates.h"

#include "generated/introspectors.h"
#include "application/config_lua_table.h"


void server_setup::wait_for_listen_server() {
	std::unique_lock<std::mutex> lck(mtx);
	while (!server_ready) cv.wait(lck);
}

std::string server_setup::endpoint::nick_and_ip() const {
	return typesafe_sprintf("%x (%x)", nickname, addr.get_readable_ip());
}

server_setup::endpoint& server_setup::get_endpoint(const augs::network::endpoint_address addr) {
	return *find_in(endpoints, addr);
}

void server_setup::deinit_endpoint(endpoint& end, const bool gracefully) {
	LOG("%x disconnected.", end.nick_and_ip());

	if (hypersomnia[end.controlled_entity].alive()) {
		scene.free_character(end.controlled_entity);
	}

	if (!gracefully) {
		choose_server(end.addr).forceful_disconnect(end.addr);
	}
	else {
		choose_server(end.addr).disconnect(end.addr);
	}
}

void server_setup::deinit_endpoint(const augs::network::endpoint_address addr, const bool gracefully) {
	deinit_endpoint(get_endpoint(addr), gracefully);
}

void server_setup::disconnect(const augs::network::endpoint_address addr, const bool gracefully) {
	deinit_endpoint(addr, gracefully);
	erase_element(endpoints, addr);
}

augs::network::server& server_setup::choose_server(augs::network::endpoint_address addr) {
	if (serv.has_endpoint(addr)) {
		return serv;
	}

	ensure(alternative_serv.has_endpoint(addr));
	return alternative_serv;
}

void server_setup::process(const config_lua_table& cfg, game_window& window, const bool start_alternative_server) {
	const auto metas_of_assets_unique = get_assets_manager().generate_logical_metas_of_assets();
	const auto& metas_of_assets = *metas_of_assets_unique;


	session_report rep;

	cosmos hypersomnia_last_snapshot(3000);

	cosmos initial_hypersomnia(3000);

	augs::fixed_delta_timer timer = augs::fixed_delta_timer(5);


	hypersomnia.load_from_file("server_save.state");
	const bool detailed_step_log = cfg.default_tickrate <= 2;

	if (cfg.get_input_recording_mode() != input_recording_type::DISABLED) {
		//if (player.try_to_load_or_save_new_session("generated/server_sessions/", "server_recorded.inputs")) {
		//	timer.set_stepping_speed_multiplier(1.f);
		//}
	}

	simulation_broadcast server_sim;

	const bool is_replaying = false;// player.is_replaying();
	LOG("Is server replaying: %x", is_replaying);
	
	const bool launch_webserver = !is_replaying && cfg.server_launch_http_daemon;
	
	bool daemon_online = false;

	if (launch_webserver) {
		daemon_online = rep.start_daemon(cfg);
	}

	if (is_replaying || serv.listen(cfg.server_port, 32)) {
		LOG("Listen server setup successful.");
	}
	else {
		LOG("Failed to setup a listen server.");
	}

	if (start_alternative_server) {
		if (is_replaying || alternative_serv.listen(cfg.alternative_port, 32)) {
			LOG("Alternative listen server setup successful.");
		}
		else {
			LOG("Failed to setup an alternative listen server.");
		}
	}

	{
		std::unique_lock<std::mutex> lck(mtx);
		server_ready = true;
		cv.notify_all();
	}

	bool reinfer = false;
	
	timer.reset_timer();
	
	randomization test_entropy_randomizer;
	
	while (!should_quit) {
		augs::machine_entropy new_entropy;

		new_entropy.local = window.collect_entropy(cfg.enable_cursor_clipping);
		new_entropy.remote = serv.collect_entropy();

		if (start_alternative_server) {
			augs::machine_entropy alt_entropy;
			alt_entropy.remote = alternative_serv.collect_entropy();
		
			new_entropy += alt_entropy;
		}
	
		process_exit(new_entropy.local);

		for (auto& net_event : new_entropy.remote) {
			if (detailed_step_log) {
				LOG("Server netevent");
			}

			const auto& address = net_event.address;

			bool should_disconnect = false;

			if (net_event.message_type == augs::network::message::type::CONNECT) {
				LOG("%x connected to server on port %x.", address.get_readable_ip(), address.get_port());

				endpoint new_endpoint;
				new_endpoint.addr = net_event.address;
				new_endpoint.commands.set_lower_limit(static_cast<size_t>(cfg.client_commands_jitter_buffer_ms / hypersomnia.get_fixed_delta().in_milliseconds()));
				endpoints.push_back(new_endpoint);
			}

			if (net_event.message_type == augs::network::message::type::DISCONNECT) {
				LOG("Disconnecting due to net event.");
				should_disconnect = true;
			}

			if (net_event.message_type == augs::network::message::type::RECEIVE) {
				auto& stream = net_event.payload;
				auto& endpoint = *find_in(endpoints, address);

				auto to_skip = net_event.messages_to_skip;

				while (stream.get_unread_bytes() > 0) {
					const bool should_skip = to_skip > 0;

					if (should_skip) {
						--to_skip;
					}

					network_command command;
					augs::read(stream, command);

					if (detailed_step_log && !should_skip)
						LOG("Server received command: %x", int(command));

					switch (command) {
					case network_command::CLIENT_WELCOME_MESSAGE: {
						std::string read_nickname;
						augs::read(stream, read_nickname);

						if (!should_skip) {
							if (endpoint.sent_welcome_message) {
								LOG("Welcome message was resent. Disconnecting.");
								should_disconnect = true;
							}
							else {
								endpoint.sent_welcome_message = true;
								endpoint.nickname = read_nickname;

								if (!should_skip) {
									LOG("%x chose nickname %x.", address.get_readable_ip(), endpoint.nickname);

									auto& complete_state = initial_hypersomnia.reserved_memory_for_serialization;
									complete_state.reset_write_pos();

									augs::write(complete_state, network_command::COMPLETE_STATE);

									cosmic_delta::encode(initial_hypersomnia, hypersomnia, complete_state);

									reinfer = true;

									endpoint.controlled_entity = scene.assign_new_character();
									augs::write(complete_state, hypersomnia[endpoint.controlled_entity].get_guid());

									choose_server(address).send_reliable(complete_state, address);
								}
							}
						}
					}

					break;

					case network_command::CLIENT_REQUESTED_ENTROPY: {
						if (!endpoint.sent_welcome_message) {
							should_disconnect = true;
						}
						else {
							guid_mapped_entropy result;
							augs::read(stream, result);

							if (!should_skip)
								//endpoint.commands.push_back(result);
								endpoint.commands.acquire_new_command(result);
						}
					}
																	break;
					default:
						LOG("Server received invalid command: %x", int(command)); stream = augs::stream();
						break;
					}
				}
			}

			if (should_disconnect) {
				disconnect(address);
			}
		}

		auto steps = timer.count_logic_steps_to_perform(hypersomnia.get_fixed_delta());

		while (steps--) {
			if (detailed_step_log) {
				LOG("Server step");
			}

			guid_mapped_entropy total_unpacked_entropy;
			
			if (cfg.debug_randomize_entropies_in_client_setup) {
				for (size_t i = 1; i < scene.characters.size(); ++i) {
					if (test_entropy_randomizer.randval(0u, cfg.debug_randomize_entropies_in_client_setup_once_every_steps) == 0u) {
						const unsigned which = test_entropy_randomizer.randval(0, 4);

						game_intent new_intent;

						switch (which) {
						case 0: new_intent.intent = intent_type::MOVE_BACKWARD; break;
						case 1: new_intent.intent = intent_type::MOVE_FORWARD; break;
						case 2: new_intent.intent = intent_type::MOVE_LEFT; break;
						case 3: new_intent.intent = intent_type::MOVE_RIGHT; break;
						//case 4: new_intent.intent = intent_type::CROSSHAIR_PRIMARY_ACTION; break;
						}

						new_intent.is_pressed = test_entropy_randomizer.randval(0, 1) == 0;

						total_unpacked_entropy.intents_per_entity[hypersomnia[scene.characters[i].id].get_guid()].push_back(new_intent);
					}
				}
			}

			erase_if(endpoints, [this](endpoint& e) {
				if (choose_server(e.addr).has_timed_out(e.addr, hypersomnia.get_fixed_delta().in_milliseconds())) {
					LOG("%x has timed out. Disconnecting.", e.nick_and_ip());
					deinit_endpoint(e);
					return true;
				}

				return false;
			});

			for (auto& e : endpoints) {
				if (!e.sent_welcome_message) {
					// start sending commands only when it is estabilished which will be the first step that the client will receive
					// that is decided by when the welcome message arrives
					continue;
				}

				guid_mapped_entropy maybe_new_client_commands;

				if (e.commands.unpack_new_command(maybe_new_client_commands)) {
					total_unpacked_entropy += maybe_new_client_commands;

					e.next_commands_accepted = true;
				}
				else {
					e.next_commands_accepted = false;
				}
			}

			for (auto& e : endpoints) {
				if (!e.sent_welcome_message) {
					// start sending commands only when it is estabilished which will be the first step that the client will receive
					// that is decided by when the welcome message arrives
					continue;
				}

				step_packaged_for_network transported_step;

				if (reinfer) {
					LOG("Ser sends resub request at step: %x", hypersomnia.get_total_steps_passed());
				}

				transported_step.step_type = step_packaged_for_network::type::NEW_ENTROPY;
				transported_step.shall_reinfer = reinfer;
				transported_step.entropy = total_unpacked_entropy;
				transported_step.next_client_commands_accepted = e.next_commands_accepted;

				augs::stream new_data;
				augs::write(new_data, network_command::PACKAGED_STEP);
				augs::write(new_data, transported_step);

				choose_server(e.addr).post_redundant(new_data, e.addr);
			}
			
			serv.send_pending_redundant();
			if(start_alternative_server) alternative_serv.send_pending_redundant();

			cosmic_entropy id_mapped_entropy(total_unpacked_entropy, hypersomnia);
			
			if (reinfer) {
				LOG("Ser: resubs at step: %x", hypersomnia.get_total_steps_passed());
				hypersomnia.complete_reinference();
				reinfer = false;
			}

			hypersomnia.advance_deterministic_schemata(
				{ id_mapped_entropy, metas_of_assets }
			);

			if (daemon_online) {
				std::string this_step_stats;
				const char* whb = "<span style=\"color:white\">";
				const char* whe = "</span>";

				const char* ipb = "<span class=\"vstype\">";
				const char* ipe = "</span>";

				this_step_stats += typesafe_sprintf("Uptime: %x%x%x seconds\n", whb, hypersomnia.get_total_time_passed_in_seconds(), whe);
				this_step_stats += typesafe_sprintf("Players online: %x%x%x", whb, endpoints.size(), whe);

				if (endpoints.size() > 0) {
					this_step_stats += "\nPlayer list:\n";
				}

				for (size_t i = 0; i < endpoints.size(); ++i) {
					const const_entity_handle character = hypersomnia[endpoints[i].controlled_entity];

					if (character.alive()) {
						const auto pos = character.get_logic_transform().pos;
						const auto vel = character.get_effective_velocity();

						this_step_stats += typesafe_sprintf("\n#%x%x%x %x (%x%x%x)\nPos: %x%x%x\nVel: %x%x%x", whb, i+1, whe, endpoints[i].nickname, ipb, endpoints[i].addr.get_readable_ip(), ipe, whb, pos, whe, whb, vel, whe);
					}

					this_step_stats += "\n";
				}

				str_ops(this_step_stats).replace_all("\n", "\n<br/>");

				rep.fetch_stats(this_step_stats);
			}
		}
	}

	if (!is_replaying) {
		rep.stop_daemon();
	}
}