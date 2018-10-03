#pragma once

#include <memory>
#include <unordered_map>

#include "../utility/structures.h"

#include "thread_item.h"

namespace winp::app{
	class object;
}

namespace winp::ui{
	class object;
	class window_surface;
}

namespace winp::message{
	class dispatcher;
}

namespace winp::thread{
	class object;

	class windows_manager{
	public:
		using m_point_type = utility::point<int>;
		using map_type = std::unordered_map<HWND, ui::window_surface *>;

		struct cache_info{
			HWND handle;
			ui::window_surface *object;
		};

		struct mouse_info{
			ui::window_surface *moused;
			ui::window_surface *dragging;

			m_point_type last_position;
			m_point_type pressed_position;

			UINT button_pressed;
			bool is_captured;
			bool is_dragging;
		};

		windows_manager();

	private:
		friend class app::object;
		friend class object;

		friend class ui::object;
		friend class ui::window_surface;

		void prepare_for_run_();

		bool is_dialog_message_(const MSG &msg) const;

		void translate_message_(const MSG &msg) const;

		void dispatch_message_(const MSG &msg) const;

		ui::window_surface *find_object_(HWND handle) const;

		void create_window_(HWND handle, CBT_CREATEWNDW &info);

		void destroy_window_(HWND handle);

		LRESULT mouse_leave_(ui::window_surface &target, UINT msg, DWORD mouse_position);

		LRESULT mouse_move_(ui::window_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		LRESULT mouse_down_(ui::window_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		LRESULT mouse_up_(ui::window_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default);

		static void init_dispatchers_();

		static void track_mouse_(HWND target, UINT flags);

		static message::dispatcher *find_dispatcher_(UINT msg);

		static LRESULT CALLBACK entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		map_type map_;
		map_type toplevel_map_;

		mutable cache_info cache_{};
		HHOOK hook_handle_ = nullptr;

		mouse_info mouse_info_{};

		static std::shared_ptr<message::dispatcher> default_dispatcher_;
		static std::unordered_map<UINT, std::shared_ptr<message::dispatcher>> dispatchers_;
	};
}
