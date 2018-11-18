#pragma once

#include "../ui/ui_window_surface.h"

#include "menu_item.h"

namespace winp::menu{
	class object : public ui::surface, public tree{
	public:
		object();

		explicit object(thread::object &thread);

		explicit object(menu::item &parent);

		explicit object(ui::window_surface &parent);

		virtual ~object();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

	protected:
		friend class menu::item;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual std::size_t get_absolute_index_of_(const menu::component &child) const override;

		virtual std::size_t get_count_() const override;

		virtual std::size_t get_absolute_index_() const;

		virtual void destruct_();

		virtual void redraw_();
	};
}
