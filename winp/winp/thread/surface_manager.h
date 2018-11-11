#pragma once

#include <memory>
#include <unordered_map>

#include "../utility/structures.h"

#include "thread_item.h"

#define WINP_WM_MOUSELEAVE		(WM_APP + 0x1)
#define WINP_WM_MOUSEENTER		(WM_APP + 0x2)

#define WINP_WM_MOUSEMOVE		(WM_APP + 0x3)
#define WINP_WM_MOUSEWHEEL		(WM_APP + 0x4)

#define WINP_WM_MOUSEDOWN		(WM_APP + 0x5)
#define WINP_WM_MOUSEUP			(WM_APP + 0x6)
#define WINP_WM_MOUSEDBLCLK		(WM_APP + 0x7)

#define WINP_WM_MOUSEDRAG		(WM_APP + 0x8)
#define WINP_WM_MOUSEDRAGBEGIN	(WM_APP + 0x9)
#define WINP_WM_MOUSEDRAGEND	(WM_APP + 0xA)

#define WINP_WM_FOCUS			(WM_APP + 0xB)
#define WINP_WM_KEY				(WM_APP + 0xC)

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

namespace winp::thread{
	class object;

	class surface_manager{
	public:
		using m_point_type = POINT;
		using m_size_type = SIZE;
		using map_type = std::unordered_map<HWND, ui::surface *>;

		struct cache_info{
			HWND handle;
			ui::surface *object;
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

		surface_manager();

	private:
		friend class app::object;
		friend class object;

		friend class ui::object;
		friend class ui::surface;
		friend class ui::window_surface;

		void prepare_for_run_();

		bool is_dialog_message_(MSG &msg) const;

		void translate_message_(MSG &msg) const;

		void dispatch_message_(MSG &msg) const;

		ui::surface *find_object_(HWND handle) const;

		void create_window_(HWND handle, CBT_CREATEWNDW &info);

		void destroy_window_(HWND handle);

		LRESULT mouse_nc_leave_(ui::io_surface &target, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_leave_(ui::io_surface &target, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_enter_(ui::io_surface &target, DWORD mouse_position);

		LRESULT mouse_nc_move_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		LRESULT mouse_move_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		LRESULT mouse_nc_down_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default);

		LRESULT mouse_down_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default);

		LRESULT mouse_nc_up_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default);

		LRESULT mouse_up_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default);

		LRESULT mouse_nc_dbl_click_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default);

		LRESULT mouse_dbl_click_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default);

		LRESULT mouse_wheel_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		LRESULT set_focus_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		LRESULT kill_focus_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		LRESULT key_(ui::io_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, bool prevent_default);

		message::dispatcher *find_dispatcher_(UINT msg);

		void track_mouse_leave_(HWND target, UINT flags);

		static LRESULT CALLBACK entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		map_type map_;
		map_type toplevel_map_;

		mutable cache_info cache_{};
		HHOOK hook_handle_ = nullptr;

		mouse_info mouse_info_{};
		surface_state state_{};

		std::shared_ptr<message::dispatcher> default_dispatcher_;
		std::unordered_map<UINT, std::shared_ptr<message::dispatcher>> dispatchers_;
	};
}
