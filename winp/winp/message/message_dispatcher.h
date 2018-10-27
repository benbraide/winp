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

		virtual void cleanup_();

		virtual void dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default, bool is_post = false);

		virtual event_result_type pre_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool &call_default);

		virtual void post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default);

		virtual void do_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default);

		virtual void do_default_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default, bool result_set);

		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default);

		template <typename event_type, typename... other_types>
		std::shared_ptr<event_type> create_event_(ui::surface *target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *result, bool call_default, other_types &&... others){
			return std::make_shared<event_type>(*target, [=](event::object &e){
				do_default_(*target, msg, wparam, lparam, *result, call_default, false);
				e.set_result_(*result);
			}, event::message::info_type{ msg, wparam, lparam }, std::forward<other_types>(others)...);
		}

		static std::size_t event_handlers_count_of_(ui::surface &target, event::manager_base &ev);

		static void fire_event_of_(ui::surface &target, event::manager_base &ev, event::object &e);

		static HWND get_handle_of_(ui::surface &target);

		static WNDPROC get_default_message_entry_of_(ui::surface &target);

		static std::list<ui::object *> &get_children_of_(ui::tree &target);

		static bool bubble_of_(event::object &e);

		static void set_flag_of_(event::object &e, unsigned int flag);

		static void remove_flag_of_(event::object &e, unsigned int flag);

		static bool default_prevented_of_(event::object &e);

		static bool propagation_stopped_of_(event::object &e);

		static bool result_set_of_(event::object &e);
	};

	class create_destroy_dispatcher : public dispatcher{
	protected:
		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) override;
	};

	class draw_dispatcher : public dispatcher{
	protected:
		virtual void cleanup_() override;

		virtual void post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) override;

		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) override;

		std::shared_ptr<event::draw> e_;
		POINT offset_{};
	};

	class mouse_dispatcher : public dispatcher{
	protected:
		virtual void cleanup_() override;

		virtual void post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) override;

		virtual event_result_type fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) override;

		virtual void resolve_(ui::io_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, event::manager_base *&ev, event::mouse::button_type &button);

		std::shared_ptr<event::mouse> e_;
		event::manager_base *ev_;
		event::mouse::button_type button_;
	};
}
