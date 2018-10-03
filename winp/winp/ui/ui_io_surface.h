#pragma once

#include "ui_visible_surface.h"

namespace winp::message{
	class mouse_dispatcher;

	class mouse_leave_dispatcher;
	class mouse_enter_dispatcher;

	class mouse_move_dispatcher;
	class mouse_wheel_dispatcher;

	class mouse_down_dispatcher;
	class mouse_up_dispatcher;
	class mouse_double_click_dispatcher;

	class mouse_enter_dispatcher;
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
			event::manager<io_surface, event::mouse> drag_end;

			event::manager<io_surface, event::mouse> drag_out;
			event::manager<io_surface, event::mouse> drag_in;
		};

		explicit io_surface(thread::object &thread);

		explicit io_surface(tree &parent);

		virtual ~io_surface();

		mouse_event_info mouse_event;

	protected:
		friend class message::mouse_dispatcher;

		virtual void set_moused_state_(bool state);

		virtual io_surface *get_top_moused_() const;

		io_surface *moused_ = nullptr;
	};
}
