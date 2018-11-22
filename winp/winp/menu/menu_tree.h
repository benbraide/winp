#pragma once

#include "menu_component.h"

namespace winp::menu{
	class item_component;
	class item;
	class separator;

	class group;
	class object;

	class tree : public component{
	public:
		virtual ~tree() = default;

		virtual item_component *find_component(UINT id, const std::function<void(item_component *)> &callback = nullptr) const = 0;

		virtual item_component *get_component_at_absolute_index(std::size_t index, const std::function<void(item_component *)> &callback = nullptr) const = 0;

	protected:
		friend class item_component;
		friend class item;
		friend class separator;

		friend class group;
		friend class object;

		virtual UINT get_types_(std::size_t index) const = 0;

		virtual UINT get_states_(std::size_t index) const = 0;

		virtual std::size_t get_absolute_index_of_(const component &child) const = 0;

		virtual item_component *find_component_(UINT id, item_component *exclude) const = 0;

		virtual item_component *get_component_at_absolute_index_(std::size_t index) const = 0;
	};
}
