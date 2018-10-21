#pragma once

#include "../ui/ui_window_surface.h"

#include "message_object.h"

namespace winp::thread{
	class surface_manager;
}

namespace winp::message{
	class dispatcher{
	public:
		using event_result_type = event::event_result_type;

	protected:
		friend class thread::surface_manager;

		virtual void dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const;

		virtual void do_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const;

		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const;

		virtual event_result_type post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const;

		static std::size_t event_handlers_count_of_(ui::surface &target, event::manager_base &ev);

		static void fire_event_of_(ui::surface &target, event::manager_base &ev, event::object &e);

		static HWND get_handle_of_(ui::surface &target);

		static WNDPROC get_default_message_entry_of_(ui::surface &target);

		static std::list<ui::object *> &get_children_of_(ui::tree &target);

		static void set_flag_of_(event::object &e, unsigned int flag);

		static void remove_flag_of_(event::object &e, unsigned int flag);

		static bool default_prevented_of_(event::object &e);

		static bool propagation_stopped_of_(event::object &e);

		static bool result_set_of_(event::object &e);
	};

	class create_destroy_dispatcher : public dispatcher{
	protected:
		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const override;
	};

	class draw_dispatcher : public dispatcher{
	protected:
		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const override;

		virtual event_result_type post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const override;

		mutable std::shared_ptr<event::draw> e_;
		mutable POINT offset_{};
	};
}
