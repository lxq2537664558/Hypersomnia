#pragma once
#include <type_traits>
#include "object_pool_id.h"

namespace augs {
	template<bool is_const, class owner_type, class value_type>
	class basic_handle {
		typedef typename std::conditional<is_const, const owner_type&, owner_type&>::type owner_reference;
		typedef typename std::conditional<is_const, const value_type&, value_type&>::type value_reference;
		typedef object_pool_id<value_type> id_type;

	public:
		owner_reference owner;
		id_type raw_id;

		basic_handle(owner_reference owner, id_type raw_id) : owner(owner), raw_id(raw_id) {}

		void unset() {
			raw_id.unset();
		}

		void set_debug_name(std::string s) {
			raw_id.set_debug_name(s);
		}

		value_reference get() const {
			return owner.get_pool().get(raw_id);
		}

		bool alive() const {
			return owner.get_pool().alive(raw_id);
		}

		bool dead() const {
			return !alive();
		}

		id_type get_id() const {
			return raw_id;
		}

		std::string get_debug_name() const {
			return raw_id.get_debug_name();
		}
	};

	template <class T>
	class object_pool;

	template<bool is_const, class T>
	using basic_object_pool_handle = basic_handle<is_const, object_pool<T>, T>;

	template<class T>
	using object_pool_handle = basic_object_pool_handle<false, T>;
	
	template<class T>
	using const_object_pool_handle = basic_object_pool_handle<true, T>;
}