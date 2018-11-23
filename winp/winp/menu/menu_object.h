#pragma once

#include "../ui/ui_window_surface.h"

#include "menu_link.h"
#include "menu_separator.h"
#include "menu_check_item.h"
#include "menu_radio_group.h"

namespace winp::menu{
	class object : public group{
	public:
		object();

		explicit object(thread::object &thread);

		explicit object(menu::item &parent);

		explicit object(ui::window_surface &parent);

		virtual ~object();

		virtual bool is_popup(const std::function<void(bool)> &callback = nullptr) const;

		event::manager<object, event::object> create_event{ *this };
		event::manager<object, event::object> destroy_event{ *this };

		event::manager<object, event::object> uninit_event{ *this };
		event::manager<object, event::object> init_event{ *this };
		event::manager<object, event::object> init_item_event{ *this };

		event::manager<object, event::object> select_event{ *this };
		event::manager<object, event::object> check_event{ *this };
		event::manager<object, event::object> uncheck_event{ *this };

	protected:
		friend class menu::item_component;
		friend class menu::item;
		friend class menu::separator;
		friend class thread::surface_manager;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual HANDLE get_handle_() const override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual LRESULT dispatch_message_(UINT msg, WPARAM wparam, LPARAM lparam, bool call_default = false) override;

		virtual std::size_t get_count_() const override;

		virtual std::size_t get_absolute_index_() const;

		virtual void update_surface_manager_(bool add);

		virtual void redraw_();

		virtual bool is_popup_() const;

		ui::surface *context_target_ = nullptr;
	};
}
