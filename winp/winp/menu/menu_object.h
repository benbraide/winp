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

		event::manager<object, event::draw_item> draw_item_event{ *this };
		event::manager<object, event::measure_item> measure_item_event{ *this };

	protected:
		friend class menu::item_component;
		friend class menu::item;
		friend class menu::separator;
		friend class thread::surface_manager;

		virtual void event_handlers_count_changed_(event::manager_base &e, std::size_t previous_count, std::size_t current_count);

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool is_created_() const override;

		virtual HANDLE get_handle_() const override;

		virtual LRESULT dispatch_message_(UINT msg, WPARAM wparam, LPARAM lparam, bool call_default = false) override;

		virtual UINT get_types_(std::size_t index) const override;

		virtual std::size_t get_count_() const override;

		virtual bool handle_parent_change_event_(event::tree &e) override;

		virtual void handle_parent_changed_event_(event::tree &e) override;

		virtual void handle_index_changed_event_(event::tree &e) override;

		virtual std::size_t get_absolute_index_() const;

		virtual bool handle_found_in_surface_manager_(HANDLE value) const;

		virtual void update_surface_manager_(bool add);

		virtual void redraw_();

		virtual bool is_popup_() const;
	};
}
