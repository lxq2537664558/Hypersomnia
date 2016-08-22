#pragma once
#include "augs/misc/streams.h"

struct _ENetAddress;
typedef struct _ENetAddress ENetAddress;

namespace augs {
	namespace network {
		typedef augs::stream packet;

		class endpoint_address {
			unsigned ip;
			unsigned short port;
		public:
			endpoint_address();
			endpoint_address(const ENetAddress& addr);

			unsigned get_ip() const;
			unsigned short get_port() const;
		};

		struct message {
			enum class type {
				INVALID,
				CONNECT,
				DISCONNECT,
				RECEIVE
			};

			type message_type = type::INVALID;
			endpoint_address address;
			packet payload;
		};
	}


	template<class A>
	void read_object(A& ar, network::message& s) {
		read_object(ar, s.message_type);
		read_object(ar, s.address);
		read_object(ar, s.payload.buf);
	}

	template<class A>
	void write_object(A& ar, const network::message& s) {
		write_object(ar, s.message_type);
		write_object(ar, s.address);
		write_object(ar, s.payload.buf);
	}
}