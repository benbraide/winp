#include "../window/frame_window.h"
#include "../message/message_dispatcher.h"
#include "../app/app_object.h"

winp::thread::surface_manager::surface_manager(){
	default_dispatcher_ = std::make_shared<message::dispatcher>();

	dispatchers_[WINP_WM_PARENT_CHANGED] = std::make_shared<message::tree_dispatcher>();
	dispatchers_[WINP_WM_INDEX_CHANGED] = std::make_shared<message::tree_dispatcher>();
	dispatchers_[WINP_WM_CHILD_INDEX_CHANGED] = std::make_shared<message::tree_dispatcher>();

	dispatchers_[WINP_WM_CHILD_INSERTED] = std::make_shared<message::tree_dispatcher>();
	dispatchers_[WINP_WM_CHILD_REMOVED] = std::make_shared<message::tree_dispatcher>();

	dispatchers_[WM_NCCREATE] = std::make_shared<message::create_destroy_dispatcher>();
	dispatchers_[WM_NCDESTROY] = std::make_shared<message::create_destroy_dispatcher>();

	dispatchers_[WM_ERASEBKGND] = std::make_shared<message::draw_dispatcher>();
	dispatchers_[WM_PAINT] = std::make_shared<message::draw_dispatcher>();
	dispatchers_[WM_PRINTCLIENT] = std::make_shared<message::draw_dispatcher>();

	dispatchers_[WINP_WM_MOUSELEAVE] = std::make_shared<message::mouse_dispatcher>();
	dispatchers_[WINP_WM_MOUSEENTER] = std::make_shared<message::mouse_dispatcher>();

	dispatchers_[WINP_WM_MOUSEMOVE] = std::make_shared<message::mouse_dispatcher>();
	dispatchers_[WINP_WM_MOUSEWHEEL] = std::make_shared<message::mouse_dispatcher>();

	dispatchers_[WINP_WM_MOUSEDOWN] = std::make_shared<message::mouse_dispatcher>();
	dispatchers_[WINP_WM_MOUSEUP] = std::make_shared<message::mouse_dispatcher>();
	dispatchers_[WINP_WM_MOUSEDBLCLK] = std::make_shared<message::mouse_dispatcher>();

	dispatchers_[WINP_WM_MOUSEDRAG] = std::make_shared<message::mouse_dispatcher>();
	dispatchers_[WINP_WM_MOUSEDRAGBEGIN] = std::make_shared<message::mouse_dispatcher>();
	dispatchers_[WINP_WM_MOUSEDRAGEND] = std::make_shared<message::mouse_dispatcher>();

	dispatchers_[WM_SETCURSOR] = std::make_shared<message::cursor_dispatcher>();
	dispatchers_[WINP_WM_FOCUS] = std::make_shared<message::focus_dispatcher>();
	dispatchers_[WINP_WM_KEY] = std::make_shared<message::key_dispatcher>();

	dispatchers_[WM_UNINITMENUPOPUP] = std::make_shared<message::menu_dispatcher>();
	dispatchers_[WM_INITMENUPOPUP] = std::make_shared<message::menu_dispatcher>();
	dispatchers_[WINP_WM_MENU_INIT_ITEM] = std::make_shared<message::menu_dispatcher>();

	dispatchers_[WINP_WM_MENU_SELECT] = std::make_shared<message::menu_dispatcher>();
	dispatchers_[WINP_WM_MENU_CHECK] = std::make_shared<message::menu_dispatcher>();
	dispatchers_[WINP_WM_MENU_UNCHECK] = std::make_shared<message::menu_dispatcher>();
}

void winp::thread::surface_manager::prepare_for_run_(){
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, GetCurrentThreadId());
}

bool winp::thread::surface_manager::is_dialog_message_(MSG &msg) const{
	if (state_.focused != nullptr)
		return state_.focused->is_dialog_message_(msg);
	return ((state_.mouse_focused == nullptr) ? false : state_.mouse_focused->is_dialog_message_(msg));
}

void winp::thread::surface_manager::translate_message_(MSG &msg) const{
	TranslateMessage(&msg);
}

void winp::thread::surface_manager::dispatch_message_(MSG &msg) const{
	if (msg.hwnd == nullptr || !is_dialog_message_(msg)){
		translate_message_(msg);
		DispatchMessageW(&msg);
	}
}

winp::ui::surface *winp::thread::surface_manager::find_object_(HANDLE handle) const{
	if (handle == cache_.handle)
		return cache_.object;

	if (map_.empty())
		return nullptr;

	auto it = map_.find(handle);
	if (it == map_.end())
		return nullptr;

	if (IsWindow(static_cast<HWND>(handle)) != FALSE){
		cache_.handle = handle;
		cache_.object = it->second;
	}

	return it->second;
}

void winp::thread::surface_manager::create_window_(HWND handle, CBT_CREATEWNDW &info){
	if (cache_.creating == nullptr || static_cast<ui::surface *>(info.lpcs->lpCreateParams) != cache_.creating)
		return;//External source

	cache_.handle = handle;
	cache_.object = cache_.creating;

	map_[handle] = cache_.creating;
	cache_.creating = nullptr;

	cache_.object->set_handle_(handle);
	cache_.object->set_message_entry_(reinterpret_cast<LONG_PTR>(entry_));
	cache_.object->add_to_toplevel_();

	auto frame_object = dynamic_cast<window::frame *>(cache_.object);
	if (frame_object != nullptr)//Update system menu
		frame_object->system_menu_.init_(GetSystemMenu(handle, FALSE));
}

LRESULT winp::thread::surface_manager::destroy_window_(ui::surface &target, const MSG &info){
	if (!toplevel_map_.empty())
		toplevel_map_.erase(info.hwnd);

	if (!map_.empty())
		map_.erase(info.hwnd);

	if (cache_.handle == info.hwnd){
		cache_.handle = nullptr;
		cache_.object = nullptr;
	}

	auto frame_object = dynamic_cast<window::frame *>(&target);
	if (frame_object != nullptr)//Destroy system menu
		frame_object->system_menu_.destroy_();

	return find_dispatcher_(info.message)->dispatch_(target, info, true);
}

LRESULT winp::thread::surface_manager::mouse_nc_leave_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	mouse_info_.tracking_mouse = false;

	auto dispatcher = find_dispatcher_(WINP_WM_MOUSELEAVE);
	for (ui::io_surface *surface = target.get_first_ancestor_of_<ui::io_surface>(), *surface_parent = nullptr; surface != nullptr; surface = surface_parent){
		if (surface->hit_test_(m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) }, true) == utility::hit_target::inside)
			break;//Mouse is inside surface

		dispatcher->dispatch_(*surface, info, false);
		if ((surface_parent = surface->get_first_ancestor_of_<ui::io_surface>()) != nullptr && surface_parent->moused_ == surface)
			surface_parent->moused_ = nullptr;//Update parent's moused

		mouse_info_.mouse_target = surface_parent;
	}

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_leave_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	mouse_info_.tracking_mouse = false;
	
	LRESULT result = 0;
	auto dispatcher = find_dispatcher_(WINP_WM_MOUSELEAVE);

	auto do_hit_test = false;
	for (ui::io_surface *surface = mouse_info_.mouse_target, *surface_parent = nullptr; surface != nullptr; surface = surface_parent){
		if (do_hit_test && surface->hit_test_(m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) }, true) == utility::hit_target::inside)
			break;//Mouse is inside surface

		if (!do_hit_test && surface == &target)
			do_hit_test = true;//Test ancestors for mouse position

		if (mouse_info_.drag_target == surface){
			find_dispatcher_(WINP_WM_MOUSEDRAGEND)->dispatch_(*mouse_info_.drag_target, MSG{ info.hwnd, WINP_WM_MOUSEDRAGEND }, false);
			mouse_info_.drag_target = nullptr;
		}

		if ((!prevent_default && surface == &target))
			result = dispatcher->dispatch_(*surface, info, true);
		else//Don't update result
			dispatcher->dispatch_(*surface, info, false);

		if ((surface_parent = surface->get_first_ancestor_of_<ui::io_surface>()) != nullptr && surface_parent->moused_ == surface)
			surface_parent->moused_ = nullptr;//Update parent's moused

		mouse_info_.mouse_target = surface_parent;
	}

	return result;
}

LRESULT winp::thread::surface_manager::mouse_enter_(ui::io_surface &target, const MSG &info, DWORD mouse_position){
	auto dispatcher = find_dispatcher_(WINP_WM_MOUSEENTER);
	m_point_type computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };

	auto surface_parent = target.get_io_surface_parent_();
	if (surface_parent != nullptr)
		surface_parent->moused_ = &target;

	MSG enter_info{ info.hwnd, WINP_WM_MOUSEENTER };
	mouse_info_.mouse_target = &target;
	dispatcher->dispatch_(target, enter_info, false);

	for (auto surface = &target; surface != nullptr; surface = surface->moused_){
		if ((surface->moused_ = surface->find_moused_child_(computed_mouse_position)) != nullptr){
			dispatcher->dispatch_(*surface->moused_, enter_info, false);
			mouse_info_.mouse_target = surface->moused_;
		}
	}

	return 0;
}

LRESULT winp::thread::surface_manager::mouse_nc_move_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	if (!mouse_info_.tracking_mouse)
		track_mouse_leave_(static_cast<HWND>(target.get_handle_()), TME_NONCLIENT);

	if (mouse_info_.drag_target == &target){
		find_dispatcher_(WINP_WM_MOUSEDRAGEND)->dispatch_(*mouse_info_.drag_target, MSG{ info.hwnd, WINP_WM_MOUSEDRAGEND, info.wParam, info.lParam }, false);
		mouse_info_.drag_target = nullptr;
	}

	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_move_(*surface_parent, info, mouse_position, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_move_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	if (!mouse_info_.tracking_mouse)
		track_mouse_leave_(static_cast<HWND>(target.get_handle_()), 0);

	m_point_type computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };
	if (mouse_info_.mouse_target != nullptr && mouse_info_.mouse_target != &target && mouse_info_.mouse_target->hit_test_(computed_mouse_position, true) != utility::hit_target::inside)
		mouse_leave_(*mouse_info_.mouse_target, MSG{ info.hwnd, WM_MOUSELEAVE, info.wParam, info.lParam }, mouse_position, true);

	if (mouse_info_.mouse_target == nullptr)
		mouse_enter_(target, info, mouse_position);
	else if ((mouse_info_.mouse_target->moused_ = mouse_info_.mouse_target->find_moused_child_(computed_mouse_position)) != nullptr)
		mouse_enter_(*mouse_info_.mouse_target->moused_, info, mouse_position);

	auto last_mouse_position = mouse_info_.last_position;
	if (mouse_info_.drag_target == nullptr){
		m_size_type delta{//Absolute values
			std::abs(computed_mouse_position.x - mouse_info_.pressed_position.x),
			std::abs(computed_mouse_position.y - mouse_info_.pressed_position.y)
		};
		               
		if ((mouse_info_.drag_target = mouse_info_.mouse_target->get_drag_target_(delta)) != nullptr){//Begin drag
			mouse_info_.last_position = mouse_info_.pressed_position;
			find_dispatcher_(WINP_WM_MOUSEDRAGBEGIN)->dispatch_(*mouse_info_.drag_target, MSG{ info.hwnd, WINP_WM_MOUSEDRAGBEGIN, info.wParam, info.lParam }, !prevent_default);
			mouse_info_.last_position = last_mouse_position;
		}
	}

	if (mouse_info_.drag_target != nullptr)//Continue drag
		find_dispatcher_(WINP_WM_MOUSEDRAG)->dispatch_(*mouse_info_.drag_target, MSG{ info.hwnd, WINP_WM_MOUSEDRAG, info.wParam, info.lParam }, !prevent_default);

	auto result = find_dispatcher_(WINP_WM_MOUSEMOVE)->dispatch_(*mouse_info_.mouse_target, info, !prevent_default);
	mouse_info_.last_position = computed_mouse_position;

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_down_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_down_(*surface_parent, info, mouse_position, button, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_down_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	if (mouse_info_.mouse_target == nullptr)
		mouse_info_.mouse_target = &target;

	state_.mouse_focused = mouse_info_.mouse_target;
	auto result = find_dispatcher_(WINP_WM_MOUSEDOWN)->dispatch_(*mouse_info_.mouse_target, info, !prevent_default);

	if (!prevent_default && mouse_info_.first_button_pressed == 0u){
		mouse_info_.pressed_position = m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };
		mouse_info_.first_button_pressed = mouse_info_.button_pressed = button;
	}
	else
		mouse_info_.button_pressed |= button;

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_up_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_up_(*surface_parent, info, mouse_position, button, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_up_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	auto result = find_dispatcher_(WINP_WM_MOUSEUP)->dispatch_(*mouse_info_.mouse_target, info, !prevent_default);

	mouse_info_.button_pressed &= ~button;
	if (!prevent_default && button == mouse_info_.first_button_pressed){
		mouse_info_.first_button_pressed = 0u;
		if (mouse_info_.drag_target != nullptr){
			find_dispatcher_(WINP_WM_MOUSEDRAGEND)->dispatch_(*mouse_info_.drag_target, MSG{ info.hwnd, WINP_WM_MOUSEDRAGEND, info.wParam, info.lParam }, false);
			mouse_info_.drag_target = nullptr;
		}
	}

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_dbl_click_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_dbl_click_(*surface_parent, info, mouse_position, button, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_dbl_click_(ui::io_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	return find_dispatcher_(WINP_WM_MOUSEDBLCLK)->dispatch_(*target.get_top_moused_(), info, !prevent_default);
}

LRESULT winp::thread::surface_manager::mouse_wheel_(ui::io_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	return find_dispatcher_(WINP_WM_MOUSEWHEEL)->dispatch_(*target.get_top_moused_(), info, !prevent_default);
}

LRESULT winp::thread::surface_manager::set_cursor_(ui::io_surface &target, const MSG &info, bool prevent_default){
	auto value = find_dispatcher_(WM_SETCURSOR)->dispatch_(target, info, !prevent_default);
	SetCursor((value == 0) ? LoadCursorW(nullptr, IDC_ARROW) : reinterpret_cast<HCURSOR>(value));
	return TRUE;
}

LRESULT winp::thread::surface_manager::set_focus_(ui::io_surface &target, const MSG &info, bool prevent_default){
	state_.focused = &target;
	return find_dispatcher_(WINP_WM_FOCUS)->dispatch_(target, info, !prevent_default);
}

LRESULT winp::thread::surface_manager::kill_focus_(ui::io_surface &target, const MSG &info, bool prevent_default){
	if (state_.focused == &target)
		state_.focused = state_.mouse_focused;

	return find_dispatcher_(WINP_WM_FOCUS)->dispatch_(target, info, !prevent_default);
}

LRESULT winp::thread::surface_manager::key_(ui::io_surface &target, const MSG &info, bool prevent_default){
	return find_dispatcher_(WINP_WM_KEY)->dispatch_(*target.get_top_moused_(), info, !prevent_default);
}

LRESULT winp::thread::surface_manager::command_(ui::surface &target, const MSG &info, bool prevent_default){
	if (info.lParam == 0){//Accelerator | Menu
		if (HIWORD(info.wParam) == 0){//Menu
			menu::object *menu;
			menu::item *item = nullptr;

			for (auto &entry : map_){
				if ((menu = dynamic_cast<menu::object *>(entry.second)) != nullptr && (item = menu->find_component_(LOWORD(info.wParam))) != nullptr)
					break;//Item found
			}

			if (item == nullptr)
				return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default);

			return menu_select_(target, MSG{ info.hwnd, info.message, static_cast<WPARAM>(item->get_absolute_index_()), reinterpret_cast<LPARAM>(static_cast<HMENU>(item->get_parent_()->get_handle_())) }, prevent_default);
		}
	}

	return 0;
}

LRESULT winp::thread::surface_manager::menu_init_(ui::surface &target, const MSG &info, bool prevent_default){
	if (find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default) != 0)
		return 0;//Default prevented

	auto menu = find_object_(reinterpret_cast<HMENU>(info.wParam));
	return ((menu == nullptr) ? 0 : menu_init_items_(target, *menu));
}

LRESULT winp::thread::surface_manager::menu_init_items_(ui::surface &target, ui::surface &tree){
	menu::item *item;
	for (auto child : tree.children_){
		if (dynamic_cast<menu::tree *>(child) != nullptr)
			menu_init_items_(target, *dynamic_cast<ui::surface *>(child));
		else if ((item = dynamic_cast<menu::item *>(child)) != nullptr)
			menu_init_item_(target, *item);
	}

	return 0;
}

void winp::thread::surface_manager::menu_init_item_(ui::surface &target, menu::item &item){
	if (find_dispatcher_(WINP_WM_MENU_INIT_ITEM)->dispatch_(target, MSG{ static_cast<HWND>(target.get_handle_()), WINP_WM_MENU_INIT_ITEM, reinterpret_cast<WPARAM>(&item) }, false) == 0)
		item.remove_state_(MFS_DISABLED);
	else//Disabled
		item.set_state_(MFS_DISABLED);
}

LRESULT winp::thread::surface_manager::menu_select_(ui::surface &target, const MSG &info, bool prevent_default){
	//Check for menu check item
	return find_dispatcher_(WINP_WM_MENU_SELECT)->dispatch_(target, info, !prevent_default);
}

winp::message::dispatcher *winp::thread::surface_manager::find_dispatcher_(UINT msg){
	auto it = dispatchers_.find(msg);
	return ((it == dispatchers_.end()) ? default_dispatcher_.get() : it->second.get());
}

void winp::thread::surface_manager::track_mouse_leave_(HWND target, UINT flags){
	if (target != nullptr){
		TRACKMOUSEEVENT info{ sizeof(TRACKMOUSEEVENT), (TME_LEAVE | flags), target, 0 };
		TrackMouseEvent(&info);
		mouse_info_.tracking_mouse = true;
	}
}

LRESULT CALLBACK winp::thread::surface_manager::entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam){
	auto &manager = app::object::get_current_thread()->surface_manager_;

	auto object = manager.find_object_(handle);
	if (object == nullptr)//Forward message
		return ((IsWindowUnicode(handle) == FALSE) ? CallWindowProcA(DefWindowProcA, handle, msg, wparam, lparam) : CallWindowProcW(DefWindowProcW, handle, msg, wparam, lparam));

	MSG info{ handle, msg, wparam, lparam };
	auto io_surface = dynamic_cast<ui::io_surface *>(object);

	switch (msg){
	case WM_NCDESTROY:
		return manager.destroy_window_(*object, info);
	case WM_COMMAND:
		return manager.command_(*object, info, false);
	case WM_INITMENUPOPUP:
		return manager.menu_init_(*object, info, false);
	case WM_MENUCOMMAND:
		return manager.menu_select_(*object, info, false);
	default:
		break;
	}

	if (io_surface != nullptr){
		switch (msg){
		case WM_NCMOUSELEAVE:
			return manager.mouse_nc_leave_(*io_surface, info, GetMessagePos(), false);
		case WM_MOUSELEAVE:
			return manager.mouse_leave_(*io_surface, info, GetMessagePos(), false);
		case WM_NCMOUSEMOVE:
			return manager.mouse_nc_move_(*io_surface, info, GetMessagePos(), false);
		case WM_MOUSEMOVE:
			return manager.mouse_move_(*io_surface, info, GetMessagePos(), false);
		case WM_LBUTTONDOWN:
			return manager.mouse_down_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_MBUTTONDOWN:
			return manager.mouse_down_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_RBUTTONDOWN:
			return manager.mouse_down_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_LBUTTONUP:
			return manager.mouse_up_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_MBUTTONUP:
			return manager.mouse_up_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_RBUTTONUP:
			return manager.mouse_up_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_LBUTTONDBLCLK:
			return manager.mouse_dbl_click_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_MBUTTONDBLCLK:
			return manager.mouse_dbl_click_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_RBUTTONDBLCLK:
			return manager.mouse_dbl_click_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_NCLBUTTONDOWN:
			return manager.mouse_nc_down_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_NCMBUTTONDOWN:
			return manager.mouse_nc_down_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_NCRBUTTONDOWN:
			return manager.mouse_nc_down_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_NCLBUTTONUP:
			return manager.mouse_nc_up_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_NCMBUTTONUP:
			return manager.mouse_nc_up_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_NCRBUTTONUP:
			return manager.mouse_nc_up_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_NCLBUTTONDBLCLK:
			return manager.mouse_nc_dbl_click_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_NCMBUTTONDBLCLK:
			return manager.mouse_nc_dbl_click_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_NCRBUTTONDBLCLK:
			return manager.mouse_nc_dbl_click_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_MOUSEWHEEL:
			return manager.mouse_wheel_(*io_surface, info, GetMessagePos(), false);
		case WM_SETCURSOR:
			return manager.set_cursor_(*io_surface, info, false);
		case WM_SETFOCUS:
			return manager.set_focus_(*io_surface, info, false);
		case WM_KILLFOCUS:
			return manager.kill_focus_(*io_surface, info, false);
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			return manager.key_(*io_surface, info, false);
		default:
			break;
		}
	}

	return manager.find_dispatcher_(msg)->dispatch_(*object,info, true);
}

LRESULT CALLBACK winp::thread::surface_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
	switch (code){
	case HCBT_CREATEWND:
		app::object::get_current_thread()->surface_manager_.create_window_(reinterpret_cast<HWND>(wparam), *reinterpret_cast<CBT_CREATEWNDW *>(lparam));
		break;
	default:
		break;
	}

	return CallNextHookEx(nullptr, code, wparam, lparam);
}
