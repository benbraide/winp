#pragma once

#include <unordered_map>

#include "menu_object.h"

namespace winp::menu{
	class wrapper : public object{
	public:
		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::unordered_map<menu::component *, item_ptr_type>;

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

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void child_removed_(ui::object &child, std::size_t previous_index) override;

		bool init_(HMENU value);

		bool wrap_(HMENU value);

		list_type item_list_;
	};
}
