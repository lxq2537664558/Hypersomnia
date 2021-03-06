#pragma once

namespace augs {
	template <class F>
	auto recursive(F&& callback) {
		return [&](auto&&... args) { 
			callback(
				std::forward<F>(callback), 
				std::forward<decltype(args)>(args)...
			);
		};
	}
}