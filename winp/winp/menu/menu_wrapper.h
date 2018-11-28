#pragma once

#include <list>
#include <unordered_map>

#include "menu_object.h"

namespace winp::menu{
	class wrapper : public object{
	public:
		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::list<item_ptr_type>;
		using map_type = std::unordered_map<menu::component *, item_ptr_type>;

		wrapper();

		explicit wrapper(thread::object &thread);

		explicit wrapper(HMENU value);

		wrapper(thread::object &thread, HMENU value);

		virtual ~wrapper();

		bool init(HMENU value, const std::function<void(wrapper &, bool)> &callback = nullptr);

	protected:
		friend class thread::surface_manager;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool handle_parent_change_event_(event::tree &e) override;

		virtual void handle_child_inserted_event_(event::tree &e) override;

		virtual void handle_child_removed_event_(event::tree &e) override;

		bool init_(HMENU value);

		bool wrap_(HMENU value);

		map_type item_map_;
		list_type marked_items_;
	};
}
