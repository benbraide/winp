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
}

void winp::thread::surface_manager::prepare_for_run_(){
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, GetCurrentThreadId());
}

bool winp::thread::surface_manager::is_dialog_message_(const MSG &msg) const{
	return false;
}

void winp::thread::surface_manager::translate_message_(const MSG &msg) const{
	TranslateMessage(&msg);
}

void winp::thread::surface_manager::dispatch_message_(const MSG &msg) const{
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

LRESULT winp::thread::surface_manager::mouse_leave_(ui::io_surface &target, UINT msg, DWORD mouse_position, bool prevent_default){
	if (!prevent_default){
		auto target_handle = target.get_handle_();
		switch (SendMessageW(target_handle, WM_NCHITTEST, 0, mouse_position)){
		case HTCLIENT://Inside client --> Possibly inside child
			if (msg == WM_NCMOUSELEAVE)//Moved from non-client to client
				track_mouse_leave_(target_handle, 0);
			return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target_handle, msg, 0, 0));
		case HTNOWHERE:
		case HTERROR://Outside window
			break;
		default://Moved from client to non-client
			track_mouse_leave_(target_handle, TME_NONCLIENT);
			return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target_handle, msg, 0, 0));
		}
	}
	
	LRESULT result = 0;
	auto dispatcher = find_dispatcher_(WINP_WM_MOUSELEAVE);

	auto do_hit_test = false;
	ui::io_surface *surface = target.get_top_moused_(), *surface_parent = nullptr;

	for (; surface != nullptr; surface = surface_parent){
		if (do_hit_test && surface->hit_test_(m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) }, true) == utility::hit_target::inside)
			break;//Mouse is inside surface

		if (!do_hit_test && surface == &target)
			do_hit_test = true;//Test ancestors for mouse position

		result = dispatcher->dispatch_(*surface, WINP_WM_MOUSELEAVE, 0, 0, (!prevent_default && surface == &target));
		if ((surface_parent = surface->get_io_surface_parent_()) != nullptr && surface_parent->moused_ == surface)
			surface_parent->moused_ = nullptr;//Update parent's moused
		else if (mouse_info_.root == surface)
			mouse_info_.root = nullptr;
	}

	return result;
}

LRESULT winp::thread::surface_manager::mouse_enter_(ui::io_surface &target, DWORD mouse_position){
	auto target_parent = target.get_io_surface_parent_();
	find_dispatcher_(WINP_WM_MOUSEENTER)->dispatch_(target, WINP_WM_MOUSEENTER, 0, 0, !false);

	if (dynamic_cast<ui::window_surface *>(&target) != nullptr){//Track mouse leave on surface with mouse
		auto target_handle = target.get_handle_();
		switch (SendMessageW(target_handle, WM_NCHITTEST, 0, mouse_position)){
		case HTCLIENT://Inside client
			track_mouse_leave_(target_handle, 0);
			break;
		default://Inside non-client
			track_mouse_leave_(target_handle, TME_NONCLIENT);
			break;
		}
	}

	auto surface_parent = target.get_io_surface_parent_();
	if (surface_parent == nullptr && dynamic_cast<ui::window_surface *>(&target) != nullptr)
		mouse_info_.root = &target;
	else if (surface_parent == nullptr)
		surface_parent->moused_ = &target;

	return 0;
}

LRESULT winp::thread::surface_manager::mouse_nc_move_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	auto result = (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), WM_NCMOUSEMOVE, wparam, lparam));

	auto io_ancestor = target.get_first_ancestor_of_<ui::io_surface>();
	if (io_ancestor != nullptr)
		mouse_move_(*io_ancestor, mouse_position, 0, 0, true);

	return result;
}

LRESULT winp::thread::surface_manager::mouse_move_(ui::io_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	auto target_parent = target.get_io_surface_parent_();
	auto previous_moused = ((target_parent == nullptr) ? mouse_info_.root : target_parent->moused_);

	m_point_type computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };
	if (previous_moused == &target && !mouse_info_.is_captured){//Check for offspring with mouse out
		for (auto moused = target.moused_; moused != nullptr; moused = moused->moused_){
			if (moused->hit_test_(computed_mouse_position, true) != utility::hit_target::inside){
				mouse_leave_(*moused, WINP_WM_MOUSELEAVE, mouse_position, true);
				break;
			}
		}
	}
	else if (previous_moused != &target && previous_moused != nullptr){//Mouse leave from previous
		mouse_leave_(*previous_moused, WINP_WM_MOUSELEAVE, mouse_position, true);
		mouse_enter_(target, mouse_position);
	}

	for (auto surface = &target; surface != nullptr; surface = surface->moused_){
		if (surface->moused_ == nullptr && (surface->moused_ = surface->find_moused_child_(computed_mouse_position)) != nullptr)
			mouse_enter_(*surface->moused_, mouse_position);
	}

	auto top_moused = target.get_top_moused_();
	if (previous_moused == &target){//Check for drag
		auto last_mouse_position = mouse_info_.last_position;
		if (!mouse_info_.is_dragging && mouse_info_.is_captured){
			m_size_type delta{//Absolute values
				((computed_mouse_position.x < mouse_info_.pressed_position.x) ? (mouse_info_.pressed_position.x - computed_mouse_position.x) : (computed_mouse_position.x - mouse_info_.pressed_position.x)),
				((computed_mouse_position.y < mouse_info_.pressed_position.y) ? (mouse_info_.pressed_position.y - computed_mouse_position.y) : (computed_mouse_position.y - mouse_info_.pressed_position.y))
			};

			if (top_moused->should_begin_drag_(delta)){//Begin drag
				mouse_info_.last_position = mouse_info_.pressed_position;
				mouse_info_.is_dragging = true;
				find_dispatcher_(WINP_WM_MOUSEDRAGBEGIN)->dispatch_(*top_moused, WINP_WM_MOUSEDRAGBEGIN, wparam, lparam, !prevent_default);
			}
		}

		if (mouse_info_.is_dragging){//Continue drag
			find_dispatcher_(WINP_WM_MOUSEDRAG)->dispatch_(*top_moused, WINP_WM_MOUSEDRAG, wparam, lparam, !prevent_default);
			mouse_info_.last_position = last_mouse_position;
		}
	}

	auto result = find_dispatcher_(WINP_WM_MOUSEMOVE)->dispatch_(*top_moused, WINP_WM_MOUSEMOVE, wparam, lparam, !prevent_default);
	mouse_info_.last_position = computed_mouse_position;

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_down_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	mouse_down_(target, msg, mouse_position, wparam, lparam, button, false);
	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), msg, wparam, lparam));
}

LRESULT winp::thread::surface_manager::mouse_down_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	bool is_client;
	if (!prevent_default){
		switch (msg){
		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
			is_client = false;
			break;
		default:
			is_client = true;
			break;
		}
	}
	else
		is_client = true;

	auto top_moused = (is_client ? target.get_top_moused_() : &target);
	auto result = find_dispatcher_(WINP_WM_MOUSEDOWN)->dispatch_(*top_moused, msg, 0, 0, (is_client && !prevent_default));

	if (!prevent_default && is_client && !mouse_info_.is_captured && dynamic_cast<ui::window_surface *>(&target) != nullptr){
		SetCapture(target.get_handle_());

		mouse_info_.is_captured = true;
		mouse_info_.captured = &target;

		mouse_info_.pressed_position = m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };
		mouse_info_.last_position = mouse_info_.pressed_position;
		mouse_info_.button_pressed = button;
	}

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_up_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	mouse_up_(target, msg, mouse_position, wparam, lparam, button, false);
	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), msg, wparam, lparam));
}

LRESULT winp::thread::surface_manager::mouse_up_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	bool is_client;
	if (!prevent_default){
		switch (msg){
		case WM_NCLBUTTONUP:
		case WM_NCMBUTTONUP:
		case WM_NCRBUTTONUP:
			is_client = false;
			break;
		default:
			is_client = true;
			break;
		}
	}
	else
		is_client = true;

	auto top_moused = (is_client ? target.get_top_moused_() : &target);
	auto result = find_dispatcher_(WINP_WM_MOUSEUP)->dispatch_(*top_moused, msg, 0, 0, (is_client && !prevent_default));

	if (!prevent_default && is_client && mouse_info_.is_captured && mouse_info_.captured == &target)
		ReleaseCapture();

	return result;
}

LRESULT winp::thread::surface_manager::mouse_nc_dbl_click_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	mouse_dbl_click_(target, msg, mouse_position, wparam, lparam, button, true);
	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), msg, wparam, lparam));
}

LRESULT winp::thread::surface_manager::mouse_dbl_click_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, UINT button, bool prevent_default){
	return find_dispatcher_(WINP_WM_MOUSEDBLCLK)->dispatch_(*target.get_top_moused_(), msg, wparam, lparam, !prevent_default);
}

LRESULT winp::thread::surface_manager::mouse_wheel_(ui::io_surface &target, UINT msg, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	return find_dispatcher_(WINP_WM_MOUSEWHEEL)->dispatch_(*target.get_top_moused_(), msg, wparam, lparam, !prevent_default);
}

LRESULT winp::thread::surface_manager::capture_released_(ui::io_surface &target, WPARAM wparam, LPARAM lparam){
	if (mouse_info_.captured == nullptr || reinterpret_cast<HWND>(lparam) == mouse_info_.captured->get_handle_())
		return CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), WM_CAPTURECHANGED, wparam, lparam);

	if (mouse_info_.is_dragging){//End drag
		mouse_info_.is_dragging = false;
		find_dispatcher_(WINP_WM_MOUSEDRAGEND)->dispatch_(*target.get_top_moused_(), WINP_WM_MOUSEDRAGEND, wparam, lparam, false);
	}

	return CallWindowProcW(target.get_default_message_entry_(), target.get_handle_(), WM_CAPTURECHANGED, wparam, lparam);
}

winp::message::dispatcher *winp::thread::surface_manager::find_dispatcher_(UINT msg){
	auto it = dispatchers_.find(msg);
	return ((it == dispatchers_.end()) ? default_dispatcher_.get() : it->second.get());
}

void winp::thread::surface_manager::track_mouse_leave_(HWND target, UINT flags){
	TRACKMOUSEEVENT info{ sizeof(TRACKMOUSEEVENT), (TME_LEAVE | flags), target, 0 };
	TrackMouseEvent(&info);
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
		case WM_MOUSELEAVE:
			return manager.mouse_leave_(*io_surface, GetMessagePos(), msg, false);
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
		case WM_CAPTURECHANGED:
			return manager.capture_released_(*io_surface, wparam, lparam);
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