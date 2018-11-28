#pragma once

#include "event_handler.h"

namespace winp::thread{
	class surface_manager;
}

namespace winp::message{
	class dispatcher;
}

namespace winp::menu{
	class item_component;
}

namespace winp::event{
	class unhandled_handler;
	class draw_handler;

	class dispatcher{
	protected:
		friend class message::dispatcher;

		virtual void dispatch_(object &e);

		static void set_result_of_(event::object &e, LRESULT value, bool always_set);

		static LRESULT get_result_of_(event::object &e);

		static bool result_set_of_(event::object &e);

		static bool default_prevented_of_(event::object &e);
	};

	class tree_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class create_destroy_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class draw_dispatcher : public dispatcher{
	protected:
		friend class unhandled_handler;
		friend class draw_handler;

		virtual void dispatch_(object &e) override;

		static void erase_background_(draw &e);
	};

	class draw_item_dispatcher : public dispatcher{
	protected:
		friend class thread::surface_manager;

		friend class draw_item;
		friend class measure_item;

		friend class unhandled_handler;
		friend class draw_item_handler;

		virtual void dispatch_(object &e) override;

		static void draw_item_(draw_item &e);

		static void measure_item_(measure_item &e);

		static void draw_item_(ui::object &item, DRAWITEMSTRUCT &info, HWND handle, HTHEME theme);

		static void draw_themed_menu_item_(menu::item_component &item, DRAWITEMSTRUCT &info, HWND handle, HTHEME theme);

		static void draw_unthemed_menu_item_(menu::item_component &item, DRAWITEMSTRUCT &info, HWND handle);

		static SIZE measure_item_(ui::object &item, HWND handle, HDC device, HTHEME theme);

		static int get_menu_item_text_offset_(HDC device, HTHEME theme);

		static int get_menu_item_text_padding_(HDC device, HTHEME theme);

		static SIZE get_menu_item_check_extent_(HDC device, HTHEME theme);

		static int get_menu_item_check_gutter_(HTHEME theme);
	};

	class cursor_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class mouse_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class focus_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class key_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class menu_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class frame_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};
}
