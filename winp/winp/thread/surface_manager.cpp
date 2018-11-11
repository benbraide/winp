#include "../message/message_dispatcher.h"
#include "../app/app_object.h"

winp::thread::surface_manager::surface_manager(){
	default_dispatcher_ = std::make_shared<message::dispatcher>();

	dispatchers_[WM_CREATE] = std::make_shared<message::create_destroy_dispatcher>();
	dispatchers_[WM_DESTROY] = std::make_shared<message::create_destroy_dispatcher>();

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

	dispatchers_[WINP_WM_FOCUS] = std::make_shared<message::focus_dispatcher>();
	dispatchers_[WINP_WM_KEY] = std::make_shared<message::key_dispatcher>();
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

winp::ui::surface *winp::thread::surface_manager::find_object_(HWND handle) const{
	if (handle == cache_.handle)
		return cache_.object;

	auto it = map_.find(handle);
	if (it == map_.end())
		return nullptr;

	cache_.handle = handle;
	cache_.object = it->second;

	return cache_.object;
}

void winp::thread::surface_manager::create_window_(HWND handle, CBT_CREATEWNDW &info){
	if (cache_.object == nullptr || cache_.handle != nullptr || static_cast<ui::window_surface *>(info.lpcs->lpCreateParams) != cache_.object)
		return;//External source

	cache_.handle = handle;
	map_[handle] = cache_.object;

	cache_.object->set_handle_(handle);
	cache_.object->set_message_entry_(reinterpret_cast<LONG_PTR>(entry_));
	cache_.object->add_to_toplevel_();
}

void winp::thread::surface_manager::destroy_window_(HWND handle){
	if (!toplevel_map_.empty())
		toplevel_map_.erase(handle);

	if (!map_.empty())
		map_.erase(handle);

	if (cache_.handle == handle){
		cache_.handle = nullptr;
		cache_.object = nullptr;
	}
}

LRESULT winp::thread::surface_manager::mouse_nc_leave_(ui::io_surface &target, DWORD mouse_position, bool prevent_default){
	mouse_info_.tracking_mouse = false;

	auto dispatcher = find_dispatcher_(WINP_WM_MOUSELEAVE);
	for (ui::io_surface *surface = target.get_first_ancestor_of_<ui::io_surface>(), *surface_parent = nullptr; surface != nullptr; surface = surface_parent){
		if (surface->hit_test_(m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) }, true) == utility::hit_target::inside)
			break;//Mouse is inside surface

		dispatcher->dispatch_(*surface, WM_MOUSELEAVE, 0, 0, false);
		if ((surface_parent = surface->get_first_ancestor_of_<ui::io_surface>()) != nullptr && surface_parent->moused_ == surface)
			surface_parent->moused_ = nullptr;//Update parent's moused

		mouse_info_.mouse_target = surface_parent;
	}

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), WM_NCMOUSELEAVE, 0, 0));
}

LRESULT winp::thread::surface_manager::mouse_leave_(ui::io_surface &target, DWORD mouse_position, bool prevent_default){
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
			find_dispatcher_(WINP_WM_MOUSEDRAGEND)->dispatch_(*mouse_info_.drag_target, WINP_WM_MOUSEDRAGEND, 0, 0, false);
			mouse_info_.drag_target = nullptr;
		}

		if ((!prevent_default && surface == &target))
			result = dispatcher->dispatch_(*surface, WM_MOUSELEAVE, 0, 0, true);
		else//Don't update result
			dispatcher->dispatch_(*surface, WM_MOUSELEAVE, 0, 0, false);

		if ((surface_parent = surface->get_first_ancestor_of_<ui::io_surface>()) != nullptr && surface_parent->moused_ == surface)
			surface_parent->moused_ = nullptr;//Update parent's moused

		mouse_info_.mouse_target = surface_parent;
	}

	return result;
}

LRESULT winp::thread::surface_manager::mouse_enter_(ui::io_surface &target, DWORD mouse_position){
	auto dispatcher = find_dispatcher_(WINP_WM_MOUSEENTER);
	m_point_type computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };

	auto surface_parent = target.get_io_surface_parent_();
	if (surface_parent != nullptr)
		surface_parent->moused_ = &target;

	mouse_info_.mouse_target = &target;
	dispatcher->dispatch_(target, WINP_WM_MOUSEENTER, 0, 0, false);

	for (auto surface = &target; surface != nullptr; surface = surface->moused_){
		if ((surface->moused_ = surface->find_moused_child_(computed_mouse_position)) != nullptr){
			dispatcher->dispatch_(*surface->moused_, WINP_WM_MOUSEENTER, 0, 0, false);
			mouse_info_.mouse_target = surface->moused_;
		}
	}

	return 0;
}

LRESULT winp::thread::surface_manager::mouse_nc_move_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	if (!mouse_info_.tracking_mouse)
		track_mouse_leave_(target.get_handle_(), TME_NONCLIENT);

	if (mouse_info_.drag_target == &target){
		find_dispatcher_(WINP_WM_MOUSEDRAGEND)->dispatch_(*mouse_info_.drag_target, WINP_WM_MOUSEDRAGEND, wparam, lparam, false);
		mouse_info_.drag_target = nullptr;
	}

	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_move_(*surface_parent, mouse_position, 0, 0, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), WM_NCMOUSEMOVE, wparam, lparam));
}

LRESULT winp::thread::surface_manager::mouse_move_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	if (!mouse_info_.tracking_mouse)
		track_mouse_leave_(target.get_handle_(), 0);

	m_point_type computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };
	if (mouse_info_.mouse_target != nullptr && mouse_info_.mouse_target->hit_test_(computed_mouse_position, true) != utility::hit_target::inside)
		mouse_leave_(*mouse_info_.mouse_target, mouse_position, true);

	if (mouse_info_.mouse_target == nullptr)
		mouse_enter_(target, mouse_position);
	else if ((mouse_info_.mouse_target->moused_ = mouse_info_.mouse_target->find_moused_child_(computed_mouse_position)) != nullptr)
		mouse_enter_(*mouse_info_.mouse_target->moused_, mouse_position);

	auto last_mouse_position = mouse_info_.last_position;
	if (mouse_info_.drag_target == nullptr){
		m_size_type delta{//Absolute values
			std::abs(computed_mouse_position.x - mouse_info_.pressed_position.x),
			std::abs(computed_mouse_position.y - mouse_info_.pressed_position.y)
		};
		               
		if ((mouse_info_.drag_target = mouse_info_.mouse_target->get_drag_target_(delta)) != nullptr){//Begin drag
			mouse_info_.last_position = mouse_info_.pressed_position;
			find_dispatcher_(WINP_WM_MOUSEDRAGBEGIN)->dispatch_(*mouse_info_.drag_target, WINP_WM_MOUSEDRAGBEGIN, wparam, lparam, !prevent_default);
			mouse_info_.last_position = last_mouse_position;
		}
	}

	if (mouse_info_.drag_target != nullptr)//Continue drag
		find_dispatcher_(WINP_WM_MOUSEDRAG)->dispatch_(*mouse_info_.drag_target, WINP_WM_MOUSEDRAG, wparam, lparam, !prevent_default);

	auto result = find_dispatcher_(WINP_WM_MOUSEMOVE)->dispatch_(*mouse_info_.mouse_target, WM_MOUSEMOVE, wparam, lparam, !prevent_default);
	mouse_info_.last_position = computed_mouse_position;

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_down_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_down_(*surface_parent, msg, mouse_position, 0, 0, button, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), msg, wparam, lparam));
}

LRESULT winp::thread::surface_manager::mouse_down_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	auto result = find_dispatcher_(WINP_WM_MOUSEDOWN)->dispatch_(*mouse_info_.mouse_target, msg, 0, 0, !prevent_default);

	state_.mouse_focused = mouse_info_.mouse_target;
	if (!prevent_default && mouse_info_.first_button_pressed == 0u){
		mouse_info_.pressed_position = m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };
		mouse_info_.first_button_pressed = mouse_info_.button_pressed = button;
	}
	else
		mouse_info_.button_pressed |= button;

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_up_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_up_(*surface_parent, msg, mouse_position, 0, 0, button, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), msg, wparam, lparam));
}

LRESULT winp::thread::surface_manager::mouse_up_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	auto result = find_dispatcher_(WINP_WM_MOUSEUP)->dispatch_(*mouse_info_.mouse_target, msg, 0, 0, !prevent_default);

	mouse_info_.button_pressed &= ~button;
	if (!prevent_default && button == mouse_info_.first_button_pressed){
		mouse_info_.first_button_pressed = 0u;
		find_dispatcher_(WINP_WM_MOUSEDRAGEND)->dispatch_(*mouse_info_.drag_target, WINP_WM_MOUSEDRAGEND, wparam, lparam, false);
		mouse_info_.drag_target = nullptr;
	}

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_dbl_click_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	auto surface_parent = target.get_first_ancestor_of_<ui::io_surface>();
	if (surface_parent != nullptr)
		mouse_dbl_click_(*surface_parent, msg, mouse_position, 0, 0, button, false);

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), msg, wparam, lparam));
}

LRESULT winp::thread::surface_manager::mouse_dbl_click_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	return find_dispatcher_(WINP_WM_MOUSEDBLCLK)->dispatch_(*target.get_top_moused_(), msg, wparam, lparam, !prevent_default);
}

LRESULT winp::thread::surface_manager::mouse_wheel_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	return find_dispatcher_(WINP_WM_MOUSEWHEEL)->dispatch_(*target.get_top_moused_(), msg, wparam, lparam, !prevent_default);
}

LRESULT winp::thread::surface_manager::set_focus_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	state_.focused = &target;
	return find_dispatcher_(WINP_WM_FOCUS)->dispatch_(target, WM_SETFOCUS, wparam, lparam, !prevent_default);
}

LRESULT winp::thread::surface_manager::kill_focus_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	if (state_.focused == &target)
		state_.focused = state_.mouse_focused;

	return find_dispatcher_(WINP_WM_FOCUS)->dispatch_(target, WM_KILLFOCUS, wparam, lparam, !prevent_default);
}

LRESULT winp::thread::surface_manager::key_(ui::io_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, bool prevent_default){
	return find_dispatcher_(WINP_WM_KEY)->dispatch_(*target.get_top_moused_(), msg, wparam, lparam, !prevent_default);
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

	auto io_surface = dynamic_cast<ui::io_surface *>(object);
	if (io_surface != nullptr){
		switch (msg){
		case WM_NCMOUSELEAVE:
			return manager.mouse_nc_leave_(*io_surface, GetMessagePos(), false);
		case WM_MOUSELEAVE:
			return manager.mouse_leave_(*io_surface, GetMessagePos(), false);
		case WM_NCMOUSEMOVE:
			return manager.mouse_nc_move_(*io_surface, GetMessagePos(), wparam, lparam, false);
		case WM_MOUSEMOVE:
			return manager.mouse_move_(*io_surface, GetMessagePos(), wparam, lparam, false);
		case WM_LBUTTONDOWN:
			return manager.mouse_down_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_LBUTTON, false);
		case WM_MBUTTONDOWN:
			return manager.mouse_down_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_MBUTTON, false);
		case WM_RBUTTONDOWN:
			return manager.mouse_down_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_RBUTTON, false);
		case WM_LBUTTONUP:
			return manager.mouse_up_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_LBUTTON, false);
		case WM_MBUTTONUP:
			return manager.mouse_up_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_MBUTTON, false);
		case WM_RBUTTONUP:
			return manager.mouse_up_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_RBUTTON, false);
		case WM_LBUTTONDBLCLK:
			return manager.mouse_dbl_click_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_LBUTTON, false);
		case WM_MBUTTONDBLCLK:
			return manager.mouse_dbl_click_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_MBUTTON, false);
		case WM_RBUTTONDBLCLK:
			return manager.mouse_dbl_click_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_RBUTTON, false);
		case WM_NCLBUTTONDOWN:
			return manager.mouse_nc_down_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_LBUTTON, false);
		case WM_NCMBUTTONDOWN:
			return manager.mouse_nc_down_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_MBUTTON, false);
		case WM_NCRBUTTONDOWN:
			return manager.mouse_nc_down_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_RBUTTON, false);
		case WM_NCLBUTTONUP:
			return manager.mouse_nc_up_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_LBUTTON, false);
		case WM_NCMBUTTONUP:
			return manager.mouse_nc_up_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_MBUTTON, false);
		case WM_NCRBUTTONUP:
			return manager.mouse_nc_up_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_RBUTTON, false);
		case WM_NCLBUTTONDBLCLK:
			return manager.mouse_nc_dbl_click_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_LBUTTON, false);
		case WM_NCMBUTTONDBLCLK:
			return manager.mouse_nc_dbl_click_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_MBUTTON, false);
		case WM_NCRBUTTONDBLCLK:
			return manager.mouse_nc_dbl_click_(*io_surface, msg, GetMessagePos(), wparam, lparam, MK_RBUTTON, false);
		case WM_MOUSEWHEEL:
			return manager.mouse_wheel_(*io_surface, msg, GetMessagePos(), wparam, lparam, false);
		case WM_SETFOCUS:
			return manager.set_focus_(*io_surface, GetMessagePos(), wparam, lparam, false);
		case WM_KILLFOCUS:
			return manager.kill_focus_(*io_surface, GetMessagePos(), wparam, lparam, false);
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			return manager.key_(*io_surface, msg, wparam, lparam, false);
		default:
			break;
		}
	}

	return manager.find_dispatcher_(msg)->dispatch_(*object, msg, wparam, lparam, true);
}

LRESULT CALLBACK winp::thread::surface_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
	switch (code){
	case HCBT_CREATEWND:
		app::object::get_current_thread()->surface_manager_.create_window_(reinterpret_cast<HWND>(wparam), *reinterpret_cast<CBT_CREATEWNDW *>(lparam));
		break;
	case HCBT_DESTROYWND:
		app::object::get_current_thread()->surface_manager_.destroy_window_(reinterpret_cast<HWND>(wparam));
		break;
	default:
		break;
	}

	return CallNextHookEx(nullptr, code, wparam, lparam);
}
