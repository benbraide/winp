#pragma once

#include "ui_visible_surface.h"

namespace winp::message{
	class mouse_dispatcher;
}

namespace winp::ui{
	class io_surface : public visible_surface{
	public:
		struct mouse_event_info{
			explicit mouse_event_info(io_surface &owner);

			event::manager<io_surface, event::mouse, mouse_event_info> leave;
			event::manager<io_surface, event::mouse, mouse_event_info> enter;

			event::manager<io_surface, event::mouse, mouse_event_info> move;
			event::manager<io_surface, event::mouse, mouse_event_info> wheel;

			event::manager<io_surface, event::mouse, mouse_event_info> down;
			event::manager<io_surface, event::mouse, mouse_event_info> up;
			event::manager<io_surface, event::mouse, mouse_event_info> double_click;

			event::manager<io_surface, event::mouse, mouse_event_info> drag;
			event::manager<io_surface, event::mouse, mouse_event_info> drag_begin;
			event::manager<io_surface, event::mouse, mouse_event_info> drag_end;
		};

		struct key_event_info{
			explicit key_event_info(io_surface &owner);

			event::manager<io_surface, event::key, key_event_info> down;
			event::manager<io_surface, event::key, key_event_info> up;
			event::manager<io_surface, event::key, key_event_info> press;
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

		virtual io_surface *get_io_surface_parent_() const;

		virtual io_surface *get_top_moused_() const;

		virtual io_surface *find_moused_child_(const m_point_type &position) const;

		virtual io_surface *get_drag_target_(const m_size_type &delta) const;

		virtual bool is_dialog_message_(MSG &msg) const;

		io_surface *moused_ = nullptr;
	};
}
