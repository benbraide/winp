#pragma once

#include "../control/control_object.h"

#include "menu_link.h"

namespace winp::menu{
	class object : public ui::io_surface, public tree{
	public:
		object();

		explicit object(thread::object &thread);

		explicit object(menu::item &parent);

		explicit object(ui::window_surface &parent);

		virtual ~object();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

		virtual bool is_popup(const std::function<void(bool)> &callback = nullptr) const;

		event::manager<object, event::object> create_event{ *this };
		event::manager<object, event::object> destroy_event{ *this };

	protected:
		friend class menu::item;
		friend class thread::surface_manager;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual LRESULT dispatch_message_(UINT msg, WPARAM wparam, LPARAM lparam, bool call_default = false) override;

		virtual std::size_t get_absolute_index_of_(const menu::component &child) const override;

		virtual std::size_t get_count_() const override;

		virtual std::size_t get_absolute_index_() const;

		virtual void destruct_();

		virtual void redraw_();

		virtual bool is_popup_() const;

		control::object window_;
	};
}
