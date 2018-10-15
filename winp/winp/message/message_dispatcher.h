#pragma once

#include "../ui/ui_window_surface.h"

#include "message_object.h"

namespace winp::thread{
	class surface_manager;
}

namespace winp::message{
	class dispatcher{
	protected:
		friend class thread::surface_manager;

		enum class event_result_type{
			nil,
			result_set,
			prevent_default,
		};

		virtual void dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const;

		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const;

		static std::size_t event_handlers_count_of_(ui::surface &target, event::manager_base &ev);

		static void fire_event_of_(ui::surface &target, event::manager_base &ev, event::object &e);

		static HWND get_handle_of_(ui::surface &target);

		static WNDPROC get_default_message_entry_of_(ui::window_surface &target);

		static std::list<ui::object *> &get_children_of_(ui::tree &target);
	};

	class create_destroy_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const override;

		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const override;
	};

	class draw_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const override;

		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const override;

		virtual event_result_type fire_event_(event::draw &e, ui::tree *target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, utility::point<int> offset) const;
	};
}
