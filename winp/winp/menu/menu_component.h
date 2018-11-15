#pragma once

#include "../ui/ui_object.h"

namespace winp::menu{
	class component{
	public:
		virtual ~component() = default;

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const = 0;

	protected:
		virtual std::size_t get_count_() const = 0;
	};
}
