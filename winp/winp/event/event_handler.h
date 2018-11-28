#pragma once

#include "event_object.h"

namespace winp::ui{
	class object;
}

namespace winp::event{
	class dispatcher;
	class tree_dispatcher;
	class create_destroy_dispatcher;
	class draw_dispatcher;
	class draw_item_dispatcher;
	class cursor_dispatcher;
	class mouse_dispatcher;
	class focus_dispatcher;
	class key_dispatcher;
	class menu_dispatcher;
	class frame_dispatcher;

	class unhandled_handler{
	public:
		virtual ~unhandled_handler() = default;

	protected:
		friend class dispatcher;
		friend class ui::object;

		virtual void handle_unhandled_event_(object &e);
	};

	class tree_handler{
	public:
		virtual ~tree_handler() = default;

	protected:
		friend class tree_dispatcher;

		virtual void handle_parent_change_event_(tree &e);

		virtual void handle_index_change_event_(tree &e);

		virtual void handle_child_index_change_event_(tree &e);

		virtual void handle_child_insert_event_(tree &e);

		virtual void handle_child_remove_event_(tree &e);
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

	class draw_item_handler{
	public:
		virtual ~draw_item_handler() = default;

	protected:
		friend class draw_item_dispatcher;

		virtual void handle_draw_item_event_(draw_item &e);

		virtual void handle_measure_item_event_(measure_item &e);
	};

	class cursor_handler{
	public:
		virtual ~cursor_handler() = default;

	protected:
		friend class cursor_dispatcher;

		virtual void handle_set_cursor_event_(cursor &e);
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

	class focus_handler{
	public:
		virtual ~focus_handler() = default;

	protected:
		friend class focus_dispatcher;

		virtual void handle_set_focus_event_(object &e);

		virtual void handle_kill_focus_event_(object &e);
	};

	class key_handler{
	public:
		virtual ~key_handler() = default;

	protected:
		friend class key_dispatcher;

		virtual bool handle_key_down_event_(key &e);

		virtual bool handle_key_up_event_(key &e);

		virtual bool handle_key_char_event_(key &e);
	};

	class menu_handler{
	public:
		virtual ~menu_handler() = default;

	protected:
		friend class menu_dispatcher;

		virtual void handle_menu_uninit_event_(object &e);

		virtual void handle_menu_init_event_(object &e);

		virtual void handle_menu_init_item_event_(object &e);

		virtual void handle_menu_select_event_(object &e);

		virtual void handle_menu_check_event_(object &e);

		virtual void handle_menu_uncheck_event_(object &e);

		virtual bool handle_context_menu_query_event_(context_menu_prefix &e);

		virtual menu::object *handle_context_menu_request_event_(context_menu_prefix &e);

		virtual void handle_context_menu_event_(context_menu &e);
	};

	class frame_handler{
	public:
		virtual ~frame_handler() = default;

	protected:
		friend class frame_dispatcher;

		virtual bool handle_close_event_(object &e);

		virtual bool handle_position_change_event_(position &e);

		virtual bool handle_size_change_event_(size &e);

		virtual void handle_maximized_event_(object &e);

		virtual void handle_minimized_event_(object &e);

		virtual void handle_position_changed_event_(object &e);

		virtual void handle_size_changed_event_(object &e);
	};
}
