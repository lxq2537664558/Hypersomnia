#pragma once
#include "augs/ensure.h"
#include "augs/templates/maybe_const.h"
#include "augs/misc/enum_boolset.h"
#include "augs/misc/trivially_copyable_pair.h"

namespace augs {
	struct introspection_access;

	template <class Enum, class T>
	class enum_associative_array_base {
	public:
		using key_type = Enum;
		using mapped_type = T;
	protected:
		using size_type = std::size_t;

		static constexpr size_type max_n = static_cast<size_type>(key_type::COUNT);
		static constexpr bool is_trivially_copyable = std::is_trivially_copyable_v<mapped_type>;
		
		static_assert(
			!is_trivially_copyable || std::is_default_constructible_v<mapped_type>,
			"No support for a type that is trivially copyable but not default constructible."
		);

		using storage_type = std::array<
			std::conditional_t<is_trivially_copyable, 
				mapped_type,
				std::aligned_storage_t<sizeof(mapped_type), alignof(mapped_type)>
			>,
			max_n
		>;

		using flagset_type = augs::enum_boolset<Enum>;
		friend struct augs::introspection_access;

		/* Introspection will fail if the storage type involves an aligned storage */
		// GEN INTROSPECTOR class augs::enum_associative_array_base class key_type class mapped_type
		flagset_type is_value_set;
		storage_type data;
		// END GEN INTROSPECTOR
		
		bool is_set(const size_type index) const {
			return is_value_set.test(static_cast<Enum>(index));
		}

		template <class... Args>
		void set(const size_type index, Args&&... args) {
			is_value_set.set(static_cast<Enum>(index));
			new (&nth(index)) mapped_type(std::forward<Args>(args)...);
		}

		const size_type find_first_set_index(size_type from) const {
			while (from < max_n && !is_set(from)) {
				++from;
			}

			return from;
		}

		auto& nth(const size_type n) {
			return *reinterpret_cast<mapped_type*>(&data[n]);
		}

		const auto& nth(const size_type n) const {
			return *reinterpret_cast<const mapped_type*>(&data[n]);
		}

	public:
		template <bool is_const>
		class basic_iterator {
			using ptr_type = maybe_const_ptr_t<is_const, enum_associative_array_base>;
			using ref_type = std::pair<const key_type, maybe_const_ref_t<is_const, mapped_type>>;
			
			ptr_type ptr = nullptr;
			size_type idx = 0;

			friend class enum_associative_array_base<Enum, T>;

		public:
			basic_iterator(const ptr_type ptr, const size_type idx) : ptr(ptr), idx(idx) {}

			basic_iterator& operator++() {
				idx = ptr->find_first_set_index(idx + 1);
				return *this;
			}

			const basic_iterator operator++(int) {
				const iterator temp = *this;
				++*this;
				return temp;
			}

			bool operator==(const basic_iterator& b) const {
				return idx == b.idx;
			}

			bool operator!=(const basic_iterator& b) const {
				return idx != b.idx;
			}

			ref_type operator*() const {
				ensure(idx < ptr->capacity());
				return { static_cast<key_type>(idx), ptr->nth(idx) };
			}
		};

		using iterator = basic_iterator<false>;
		using const_iterator = basic_iterator<true>;

		template <bool>
		friend class basic_iterator;

		iterator begin() {
			return { this, find_first_set_index(0u) };
		}

		iterator end() {
			return { this, max_n };
		}

		const_iterator begin() const {
			return { this, find_first_set_index(0u) };
		}

		const_iterator end() const {
			return { this, max_n };
		}

		iterator find(const key_type enum_idx) {
			const size_type i = static_cast<size_type>(enum_idx);
			ensure(i < capacity());

			if (is_set(i)) {
				return { this, i };
			}

			return end();
		}

		template <class... T>
		trivially_copyable_pair<iterator, bool> try_emplace(const key_type k, T&&... t) {
			if (is_set(static_cast<size_type>(k))) {
				return { find(k), false };
			}
			else {
				set(static_cast<size_type>(k), std::forward<T>(t)...);
				return { find(k), true };
			}
		}

		template <class T>
		auto emplace(const key_type k, T&& t) {
			return try_emplace(k, std::forward<T>(t));
		}

		const_iterator find(const key_type enum_idx) const {
			const size_type i = static_cast<size_type>(enum_idx);
			ensure(i < capacity());

			if (is_set(i)) {
				return { this, i };
			}

			return end();
		}

		mapped_type& at(const key_type enum_idx) {
			const auto i = static_cast<size_type>(enum_idx);
			ensure(i < capacity());
			ensure(is_set(i));
			return nth(i);
		}

		const mapped_type& at(const key_type enum_idx) const {
			const auto i = static_cast<size_type>(enum_idx);
			ensure(i < capacity());
			ensure(is_set(i));
			return nth(i);
		}

		template <class = std::enable_if_t<std::is_default_constructible_v<mapped_type>>>
		mapped_type& operator[](const key_type enum_idx) {
			const auto i = static_cast<size_type>(enum_idx);
			ensure(i < capacity());

			if (!is_set(i)) {
				set(i);
			}

			return nth(i);
		}

		constexpr size_type capacity() const {
			return max_n;
		}

		constexpr size_type max_size() const {
			return max_n;
		}

		void clear() {
			if constexpr(!is_trivially_copyable) {
				for (auto& v : *this) {
					v.second.~mapped_type();
				}
			}

			is_value_set = flagset_type();
		}
	};

	template <class Enum, class T, class = void>
	class enum_associative_array;
	
	// GEN INTROSPECTOR class augs::enum_associative_array class key_type class mapped_type class dummy
	// INTROSPECT BASE augs::enum_associative_array_base<key_type, mapped_type>
	// END GEN INTROSPECTOR

	template <class Enum, class T>
	class enum_associative_array<Enum, T, std::enable_if_t<std::is_trivially_copyable_v<T>>>
		: public enum_associative_array_base<Enum, T> {
	public:
	};

	template <class Enum, class T>
	class enum_associative_array<Enum, T, std::enable_if_t<!std::is_trivially_copyable_v<T>>>
		: public enum_associative_array_base<Enum, T> {
	public:
		enum_associative_array() = default;
		
		enum_associative_array(const enum_associative_array& b) {
			for (auto& v : b) {
				emplace(v.first, v.second);
			}
		}

		enum_associative_array& operator=(const enum_associative_array& b) {
			clear();

			for (auto& v : b) {
				emplace(v.first, v.second);
			}

			return *this;
		}

		enum_associative_array(enum_associative_array&& b) {
			for (auto& v : b) {
				emplace(v.first, std::move(v.second));
			}

			b.is_value_set = flagset_type();
		}

		enum_associative_array& operator=(enum_associative_array&& b) {
			clear();

			for (auto& v : b) {
				emplace(v.first, std::move(v.second));
			}

			b.is_value_set = flagset_type();

			return *this;
		}

		~enum_associative_array() {
			clear();
		}
	};
}