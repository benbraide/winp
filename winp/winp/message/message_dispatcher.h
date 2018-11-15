#pragma once

#include "../ui/ui_window_surface.h"
#include "../event/event_dispatcher.h"

#include "message_object.h"

namespace winp::thread{
	class surface_manager;
}

namespace winp::event{
	class dispatcher;
}

namespace winp::message{
	class dispatcher{
	public:
		dispatcher();

	protected:
		friend class event::dispatcher;
		friend class thread::surface_manager;
		friend class ui::object;

		dispatcher(bool);

		virtual void cleanup_();

		virtual LRESULT dispatch_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default, bool is_post = false);

		virtual void pre_dispatch_(event::object &e, bool &call_default);

		virtual void post_dispatch_(event::object &e);

		virtual void do_dispatch_(event::object &e, bool call_default);

		virtual void do_default_(event::object &e, bool call_default);

		virtual LRESULT call_default_(event::object &e);

		virtual void fire_event_(event::object &e);

		virtual std::shared_ptr<event::object> create_event_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default);

		template <typename event_type, typename... other_types>
		std::shared_ptr<event_type> create_new_event_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default, other_types &&... others){
			return std::make_shared<event_type>(target, [this, call_default](event::object &e){
				do_default_(e, call_default);
			}, event::object::info_type{ msg, wparam, lparam }, std::forward<other_types>(others)...);
		}

		static std::size_t event_handlers_count_of_(ui::object &target, event::manager_base &ev);

		static void fire_event_of_(ui::object &target, event::manager_base &ev, event::object &e);

		static HANDLE get_handle_of_(ui::object &target);

		static WNDPROC get_default_message_entry_of_(ui::object &target);

		template <typename target_type>
		static target_type *get_first_ancestor_of_(ui::object &target){
			return target.get_first_ancestor_of_<target_type>();
		}

		static std::list<ui::object *> &get_children_of_(ui::tree &target);

		static void set_result_of_(event::object &e, LRESULT value, bool always_set);

		static LRESULT get_result_of_(event::object &e);

		static bool bubble_of_(event::object &e);

		template <typename target_type>
		static bool bubble_to_type_of_(event::object &e){
			return e.bubble_to_type_<target_type>();
		}

		static void set_flag_of_(event::object &e, unsigned int flag);

		static void remove_flag_of_(event::object &e, unsigned int flag);

		static bool default_prevented_of_(event::object &e);

		static bool propagation_stopped_of_(event::object &e);

		static bool result_set_of_(event::object &e);

		bool is_doing_default_ = false;
		std::shared_ptr<event::dispatcher> event_dispatcher_;
	};

	class tree_dispatcher : public dispatcher{
	public:
		tree_dispatcher();

	protected:
		virtual void fire_event_(event::object &e) override;

		virtual std::shared_ptr<event::object> create_event_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default) override;
	};

	class create_destroy_dispatcher : public dispatcher{
	public:
		create_destroy_dispatcher();

	protected:
		virtual void fire_event_(event::object &e) override;
	};

	class draw_dispatcher : public dispatcher{
	public:
		draw_dispatcher();

	protected:
		virtual void cleanup_() override;

		virtual void post_dispatch_(event::object &e) override;

		virtual void fire_event_(event::object &e) override;

		virtual std::shared_ptr<event::object> create_event_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default) override;

		std::shared_ptr<event::draw> e_;
		POINT offset_{};
	};

	class cursor_dispatcher : public dispatcher{
	public:
		cursor_dispatcher();

	protected:
		virtual LRESULT call_default_(event::object &e) override;

		virtual void fire_event_(event::object &e) override;

		virtual std::shared_ptr<event::object> create_event_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default) override;

		virtual HCURSOR get_default_cursor_(event::cursor &e) const;
	};

	class mouse_dispatcher : public dispatcher{
	public:
		mouse_dispatcher();

	protected:
		virtual void cleanup_() override;

		virtual void post_dispatch_(event::object &e) override;

		virtual void fire_event_(event::object &e) override;

		virtual std::shared_ptr<event::object> create_event_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default) override;

		virtual void resolve_(ui::io_surface &target, UINT msg);

		std::shared_ptr<event::mouse> e_;
		event::manager_base *ev_;
		event::mouse::button_type button_;
	};

	class focus_dispatcher : public dispatcher{
	public:
		focus_dispatcher();

	protected:
		virtual void fire_event_(event::object &e) override;
	};

	class key_dispatcher : public dispatcher{
	public:
		key_dispatcher();

	protected:
		virtual void cleanup_() override;

		virtual void post_dispatch_(event::object &e) override;

		virtual void fire_event_(event::object &e) override;

		virtual std::shared_ptr<event::object> create_event_(ui::object &target, UINT msg, WPARAM wparam, LPARAM lparam, bool call_default) override;

		virtual void resolve_(ui::io_surface &target, UINT msg);

		std::shared_ptr<event::key> e_;
		event::manager_base *ev_;
	};
}
