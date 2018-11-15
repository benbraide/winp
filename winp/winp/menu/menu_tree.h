#pragma once

#include "menu_component.h"

namespace winp::menu{
	class item;
	class group;

	class tree : public component{
	public:
		virtual ~tree() = default;

	protected:
		friend class item;
		friend class group;

		virtual std::size_t get_absolute_index_of_(const component &child) const = 0;
	};
}
