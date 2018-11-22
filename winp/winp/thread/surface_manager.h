#pragma once

#include <memory>
#include <unordered_map>

#include "../utility/structures.h"

#include "thread_item.h"

#define WINP_WM_MOUSELEAVE				(WM_APP + 0x01)
#define WINP_WM_MOUSEENTER				(WM_APP + 0x02)

#define WINP_WM_MOUSEMOVE				(WM_APP + 0x03)
#define WINP_WM_MOUSEWHEEL				(WM_APP + 0x04)

#define WINP_WM_MOUSEDOWN				(WM_APP + 0x05)
#define WINP_WM_MOUSEUP					(WM_APP + 0x06)
#define WINP_WM_MOUSEDBLCLK				(WM_APP + 0x07)

#define WINP_WM_MOUSEDRAG				(WM_APP + 0x08)
#define WINP_WM_MOUSEDRAGBEGIN			(WM_APP + 0x09)
#define WINP_WM_MOUSEDRAGEND			(WM_APP + 0x0A)

#define WINP_WM_FOCUS					(WM_APP + 0x0B)
#define WINP_WM_KEY						(WM_APP + 0x0C)

#define WINP_WM_PARENT_CHANGED			(WM_APP + 0x0D)
#define WINP_WM_INDEX_CHANGED			(WM_APP + 0x0E)
#define WINP_WM_CHILD_INDEX_CHANGED		(WM_APP + 0x0F)

#define WINP_WM_CHILD_INSERTED			(WM_APP + 0x10)
#define WINP_WM_CHILD_REMOVED			(WM_APP + 0x11)

#define WINP_WM_MENU_INIT_ITEM			(WM_APP + 0x12)
#define WINP_WM_MENU_SELECT				(WM_APP + 0x13)
#define WINP_WM_MENU_CHECK				(WM_APP + 0x14)
#define WINP_WM_MENU_UNCHECK			(WM_APP + 0x15)

#define WINP_WM_CONTEXT_MENU_QUERY		(WM_APP + 0x16)
#define WINP_WM_CONTEXT_MENU_REQUEST	(WM_APP + 0x17)

namespace winp::app{
	class object;
}

namespace winp::ui{
	class object;
	class surface;
	class io_surface;
	class window_surface;
}

namespace winp::message{
	class dispatcher;
}

namespace winp::menu{
	class item_component;
	class item;
	class object;

	template <class base_type>
	class generic_collection;

	using collection = generic_collection<object>;
}

namespace winp::thread{
	class object;

	class surface_manager{
	public:
		using m_point_type = POINT;
		using m_size_type = SIZE;
		using map_type = std::unordered_map<HANDLE, ui::surface *>;

		struct cache_info{
			HANDLE handle;
			ui::surface *object;
			ui::surface *creating;

			menu::object *active_menu;
			std::shared_ptr<menu::collection> context_menu;
		};

		struct mouse_info{
			ui::io_surface *mouse_target;
			ui::io_surface *drag_target;

			m_point_type last_position;
			m_point_type pressed_position;

			UINT first_button_pressed;
			UINT button_pressed;

			bool tracking_mouse;
		};

		struct surface_state{
			ui::io_surface *focused;
			ui::io_surface *mouse_focused;
		};

		struct context_menu_targets_info{
			ui::surface *surface;
			menu::generic_collection<menu::object> *menu;
		};

		surface_manager();

	private:
		friend class app::object;
		friend class object;

		friend class ui::object;
		friend class ui::surface;
		friend class ui::window_surface;

		friend class message::dispatcher;
		friend class menu::object;

		void prepare_for_run_();

		bool is_dialog_message_(MSG &msg) const;

		void translate_message_(MSG &msg) const;

		void dispatch_message_(MSG &msg) const;

		ui::surface *find_object_(HANDLE handle) const;

		void create_window_(HWND handle, CBT_CREATEWNDW &info);

		LRESULT destroy_window_(ui::surface &target, const MSG &info);

		LRESULT mouse_nc_leave_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_leave_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_enter_(ui::io_surface &target, const MSG &info, DWORD mouse_position);

		LRESULT mouse_nc_move_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_move_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_nc_down_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT mouse_down_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT mouse_nc_up_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT mouse_up_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT mouse_nc_dbl_click_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT mouse_dbl_click_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT mouse_wheel_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT set_cursor_(ui::io_surface &target, const MSG &info, bool prevent_default);

		LRESULT set_focus_(ui::io_surface &target, const MSG &info, bool prevent_default);

		LRESULT kill_focus_(ui::io_surface &target, const MSG &info, bool prevent_default);

		LRESULT key_(ui::io_surface &target, const MSG &info, bool prevent_default);

		LRESULT command_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT system_command_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_uninit_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_init_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_init_items_(ui::surface &target, ui::surface &tree);

		void menu_init_item_(ui::surface &target, menu::item &item);

		LRESULT menu_select_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_select_(ui::surface &target, const MSG &info, menu::item_component &item, bool prevent_default);

		LRESULT context_menu_(ui::io_surface &target, const MSG &info, bool prevent_default);

		message::dispatcher *find_dispatcher_(UINT msg);

		void track_mouse_leave_(HWND target, UINT flags);

		static LRESULT CALLBACK entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		map_type map_;
		map_type toplevel_map_;

		mutable cache_info cache_{};
		HHOOK hook_handle_ = nullptr;
		wchar_t buffer_[256];

		mouse_info mouse_info_{};
		surface_state state_{};

		std::shared_ptr<message::dispatcher> default_dispatcher_;
		std::unordered_map<UINT, std::shared_ptr<message::dispatcher>> dispatchers_;
	};
}
