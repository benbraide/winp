#pragma once

#include "menu_component.h"

namespace winp::menu{
	class item;
	class separator;
	class group;
	class object;

	class tree : public component{
	public:
		virtual ~tree() = default;

		virtual item *find_component(WORD id, const std::function<void(item *)> &callback = nullptr) const = 0;

		virtual component *get_component_at_absolute_index(std::size_t index, const std::function<void(component *)> &callback = nullptr) const = 0;

	protected:
		friend class item;
		friend class separator;
		friend class group;
		friend class object;

		virtual UINT get_types_(std::size_t index) const = 0;

		virtual UINT get_states_(std::size_t index) const = 0;

		virtual std::size_t get_absolute_index_of_(const component &child) const = 0;

		virtual item *find_component_(WORD id) const = 0;

		virtual component *get_component_at_absolute_index_(std::size_t index) const = 0;
	};
}
