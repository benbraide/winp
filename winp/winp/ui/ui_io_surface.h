#pragma once

#include "ui_visible_surface.h"

namespace winp::message{
	class mouse_dispatcher;
}

namespace winp::ui{
	class io_surface : public visible_surface{
	public:
		struct mouse_event_info{
			event::manager<io_surface, event::mouse> leave;
			event::manager<io_surface, event::mouse> enter;

			event::manager<io_surface, event::mouse> move;
			event::manager<io_surface, event::mouse> wheel;

			event::manager<io_surface, event::mouse> down;
			event::manager<io_surface, event::mouse> up;
			event::manager<io_surface, event::mouse> double_click;

			event::manager<io_surface, event::mouse> drag;
			event::manager<io_surface, event::mouse> drag_begin;
			event::manager<io_surface, event::mouse> drag_end;
		};

		struct key_event_info{
			event::manager<io_surface, event::key> down;
			event::manager<io_surface, event::key> up;
			event::manager<io_surface, event::key> press;
		};

		io_surface();

		explicit io_surface(thread::object &thread);

		virtual ~io_surface();

		mouse_event_info mouse_event;
		key_event_info key_event;

		event::manager<io_surface, event::cursor> set_cursor_event;
		event::manager<io_surface, event::object> set_focus_event;
		event::manager<io_surface, event::object> kill_focus_event;

	protected:
		friend class message::mouse_dispatcher;
		friend class thread::surface_manager;

		void init_();

		virtual io_surface *get_io_surface_parent_() const;

		virtual io_surface *get_top_moused_() const;

		virtual io_surface *find_moused_child_(const m_point_type &position) const;

		virtual io_surface *get_drag_target_(const m_size_type &delta) const;

		virtual bool is_dialog_message_(MSG &msg) const;

		io_surface *moused_ = nullptr;
	};
}
