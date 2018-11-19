#pragma once

#include "menu_component.h"

namespace winp::menu{
	class item;
	class separator;
	class group;

	class tree : public component{
	public:
		virtual ~tree() = default;

	protected:
		friend class item;
		friend class separator;
		friend class group;

		virtual UINT get_types_(std::size_t index) const = 0;

		virtual UINT get_states_(std::size_t index) const = 0;

		virtual std::size_t get_absolute_index_of_(const component &child) const = 0;
	};
}
