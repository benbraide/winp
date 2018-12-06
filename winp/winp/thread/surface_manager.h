#pragma once

#include <memory>
#include <unordered_map>

#include "../utility/structures.h"

#include "thread_item.h"

#define WINP_WM_MOUSELEAVE				(WM_APP + 0x00)
#define WINP_WM_MOUSEENTER				(WM_APP + 0x01)
#define WINP_WM_MOUSEMOVE				(WM_APP + 0x02)
#define WINP_WM_MOUSEWHEEL				(WM_APP + 0x03)

#define WINP_WM_MOUSEDOWN				(WM_APP + 0x04)
#define WINP_WM_MOUSEUP					(WM_APP + 0x05)
#define WINP_WM_MOUSEDBLCLK				(WM_APP + 0x06)

#define WINP_WM_MOUSEDRAG				(WM_APP + 0x07)
#define WINP_WM_MOUSEDRAGQUERY			(WM_APP + 0x08)
#define WINP_WM_MOUSEDRAGBEGIN			(WM_APP + 0x09)
#define WINP_WM_MOUSEDRAGEND			(WM_APP + 0x0A)

#define WINP_WM_FOCUS					(WM_APP + 0x0B)
#define WINP_WM_KEY						(WM_APP + 0x0C)

#define WINP_WM_PARENT_CHANGING			(WM_APP + 0x0D)
#define WINP_WM_INDEX_CHANGING			(WM_APP + 0x0E)
#define WINP_WM_CHILD_INDEX_CHANGING	(WM_APP + 0x0F)

#define WINP_WM_CHILD_INSERTING			(WM_APP + 0x10)
#define WINP_WM_CHILD_REMOVING			(WM_APP + 0x11)

#define WINP_WM_PARENT_CHANGED			(WM_APP + 0x12)
#define WINP_WM_INDEX_CHANGED			(WM_APP + 0x13)
#define WINP_WM_CHILD_INDEX_CHANGED		(WM_APP + 0x14)

#define WINP_WM_CHILD_INSERTED			(WM_APP + 0x15)
#define WINP_WM_CHILD_REMOVED			(WM_APP + 0x16)

#define WINP_WM_MENU_INIT_ITEM			(WM_APP + 0x17)
#define WINP_WM_MENU_SELECT				(WM_APP + 0x18)
#define WINP_WM_MENU_CHECK				(WM_APP + 0x19)
#define WINP_WM_MENU_UNCHECK			(WM_APP + 0x1A)

#define WINP_WM_CONTEXT_MENU_QUERY		(WM_APP + 0x1B)
#define WINP_WM_CONTEXT_MENU_REQUEST	(WM_APP + 0x1C)

#define WINP_WM_ERASE_BACKGROUND		(WM_APP + 0x1D)
#define WINP_WM_PAINT					(WM_APP + 0x1E)

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

namespace winp::event{
	class draw;
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
		using m_rect_type = RECT;

		using map_type = std::unordered_map<HANDLE, ui::surface *>;
		using id_map_type = std::unordered_map<UINT, ui::surface *>;

		struct cache_info{
			HANDLE handle;
			ui::surface *object;
			ui::surface *creating;
			std::shared_ptr<menu::collection> context_menu;
		};

		struct mouse_info{
			ui::window_surface *mouse_target;
			bool tracking_mouse;
		};

		struct surface_state{
			ui::window_surface *focused;
			ui::window_surface *mouse_focused;
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

		friend class event::draw;
		friend class message::dispatcher;

		friend class menu::item_component;
		friend class menu::item;
		friend class menu::object;

		void prepare_for_run_();

		bool is_dialog_message_(MSG &msg) const;

		void translate_message_(MSG &msg) const;

		void dispatch_message_(MSG &msg) const;

		ui::surface *find_object_(HANDLE handle) const;

		ui::surface *find_item_(UINT id) const;

		menu::item_component *find_system_menu_item_(ui::surface &target, UINT id) const;

		void create_window_(HWND handle, CBT_CREATEWNDW &info);

		LRESULT destroy_window_(ui::surface &target, const MSG &info);

		LRESULT draw_(ui::surface &target, const MSG &info, bool prevent_default, m_rect_type update_region);

		LRESULT mouse_nc_leave_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_leave_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_nc_move_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT mouse_move_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default);

		LRESULT nc_mouse_other_(ui::window_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT mouse_other_(ui::window_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default);

		LRESULT set_cursor_(ui::window_surface &target, const MSG &info, bool prevent_default);

		LRESULT set_focus_(ui::window_surface &target, const MSG &info, bool prevent_default);

		LRESULT kill_focus_(ui::window_surface &target, const MSG &info, bool prevent_default);

		LRESULT key_(ui::window_surface &target, const MSG &info, bool prevent_default);

		LRESULT command_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT system_command_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_uninit_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_init_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_init_items_(ui::surface &target, ui::surface &tree);

		void menu_init_item_(ui::surface &target, menu::item_component &item);

		LRESULT menu_select_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT menu_select_(ui::surface &target, const MSG &info, menu::item_component &item, bool prevent_default);

		LRESULT select_menu_item_(UINT msg, menu::item &item, ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states);

		LRESULT context_menu_(ui::window_surface &target, const MSG &info, bool prevent_default);

		LRESULT draw_item_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT measure_item_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT close_frame_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT size_frame_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT sized_frame_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT move_frame_(ui::surface &target, const MSG &info, bool prevent_default);

		LRESULT moved_frame_(ui::surface &target, const MSG &info, bool prevent_default);

		void track_mouse_leave_(HWND target, UINT flags);

		static bool menu_item_id_is_reserved_(UINT id);

		static bool initialize_dispatchers_();

		static message::dispatcher *find_dispatcher_(UINT msg);

		static LRESULT CALLBACK entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		map_type map_;
		map_type toplevel_map_;
		id_map_type id_map_;

		mutable cache_info cache_{};
		HHOOK hook_handle_ = nullptr;
		wchar_t buffer_[256];

		mouse_info mouse_info_{};
		surface_state state_{};

		static std::shared_ptr<message::dispatcher> default_dispatcher_;
		static std::unordered_map<UINT, std::shared_ptr<message::dispatcher>> dispatchers_;
		static bool unused_;
	};
}
