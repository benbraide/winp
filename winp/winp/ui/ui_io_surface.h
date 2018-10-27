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

		explicit io_surface(thread::object &thread);

		virtual ~io_surface();

		mouse_event_info mouse_event;

	protected:
		friend class message::mouse_dispatcher;
		friend class thread::surface_manager;

		virtual io_surface *get_io_surface_parent_() const;

		virtual io_surface *get_top_moused_() const;

		virtual io_surface *find_moused_child_(const m_point_type &position) const;

		virtual bool should_begin_drag_(const m_size_type &delta) const;

		io_surface *moused_ = nullptr;
	};
}
