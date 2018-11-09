#pragma once

#include "event_object.h"

namespace winp::ui{
	class object;
}

namespace winp::event{
	class dispatcher;
	class create_destroy_dispatcher;
	class draw_dispatcher;
	class mouse_dispatcher;

	class unhandled_handler{
	public:
		virtual ~unhandled_handler() = default;

	protected:
		friend class dispatcher;
		friend class ui::object;

		virtual void handle_unhandled_event_(object &e);
	};

	class create_destroy_handler{
	public:
		virtual ~create_destroy_handler() = default;

	protected:
		friend class create_destroy_dispatcher;

		virtual void handle_create_event_(object &e);

		virtual void handle_destroy_event_(object &e);
	};

	class draw_handler{
	public:
		virtual ~draw_handler() = default;

	protected:
		friend class draw_dispatcher;

		virtual void handle_background_erase_event_(draw &e);

		virtual void handle_paint_event_(draw &e);
	};

	class mouse_handler{
	public:
		virtual ~mouse_handler() = default;

	protected:
		friend class mouse_dispatcher;

		virtual void handle_mouse_leave_event_(mouse &e);

		virtual void handle_mouse_enter_event_(mouse &e);

		virtual void handle_mouse_move_event_(mouse &e);

		virtual void handle_mouse_wheel_event_(mouse &e);

		virtual void handle_mouse_down_event_(mouse &e);

		virtual void handle_mouse_up_event_(mouse &e);

		virtual void handle_mouse_double_click_event_(mouse &e);

		virtual void handle_mouse_drag_event_(mouse &e);

		virtual void handle_mouse_drag_begin_event_(mouse &e);

		virtual void handle_mouse_drag_end_event_(mouse &e);
	};
}
