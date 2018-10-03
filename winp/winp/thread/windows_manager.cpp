#include "../message/message_dispatcher.h"
#include "../app/app_object.h"

winp::thread::windows_manager::windows_manager(){}

void winp::thread::windows_manager::prepare_for_run_(){
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, GetCurrentThreadId());
}

bool winp::thread::windows_manager::is_dialog_message_(const MSG &msg) const{
	return false;
}

void winp::thread::windows_manager::translate_message_(const MSG &msg) const{
	TranslateMessage(&msg);
}

void winp::thread::windows_manager::dispatch_message_(const MSG &msg) const{
	if (msg.hwnd == nullptr || !is_dialog_message_(msg)){
		translate_message_(msg);
		DispatchMessageW(&msg);
	}
}

winp::ui::window_surface *winp::thread::windows_manager::find_object_(HWND handle) const{
	if (handle == cache_.handle)
		return cache_.object;

	auto it = map_.find(handle);
	if (it == map_.end())
		return nullptr;

	cache_.handle = handle;
	cache_.object = it->second;

	return cache_.object;
}

void winp::thread::windows_manager::create_window_(HWND handle, CBT_CREATEWNDW &info){
	if (cache_.object == nullptr || cache_.handle != nullptr || static_cast<ui::window_surface *>(info.lpcs->lpCreateParams) != cache_.object)
		return;//External source

	cache_.handle = handle;
	map_[handle] = cache_.object;

	cache_.object->set_handle_(handle);
	cache_.object->set_message_entry_(reinterpret_cast<LONG_PTR>(entry_));
	cache_.object->add_to_toplevel_();
}

void winp::thread::windows_manager::destroy_window_(HWND handle){
	if (!toplevel_map_.empty())
		toplevel_map_.erase(handle);

	if (!map_.empty())
		map_.erase(handle);

	if (cache_.handle == handle){
		cache_.handle = nullptr;
		cache_.object = nullptr;
	}
}

LRESULT winp::thread::windows_manager::mouse_leave_(ui::window_surface &target, UINT msg, DWORD mouse_position){
	auto target_handle = target.get_handle_();
	switch (SendMessageW(target_handle, WM_NCHITTEST, 0, mouse_position)){
	case HTCLIENT://Inside client --> Possibly inside child
		if (msg == WM_NCMOUSELEAVE)//Moved from non-client to client
			track_mouse_(target_handle, 0);
		return CallWindowProcW(target.get_default_message_entry_(), target_handle, msg, 0, 0);
	case HTNOWHERE:
	case HTERROR://Outside window
		break;
	default://Moved from client to non-client
		track_mouse_(target_handle, TME_NONCLIENT);
		return CallWindowProcW(target.get_default_message_entry_(), target_handle, msg, 0, 0);
	}

	
	LRESULT result = 0;
	auto dispatcher = find_dispatcher_(WM_NCMOUSELEAVE);

	ui::surface *moused = mouse_info_.moused;
	mouse_info_.moused = nullptr;

	for (; moused != nullptr && moused != &target; moused = moused->get_surface_parent_()){
		if (moused->handles_message_(WM_NCMOUSELEAVE))
			default_dispatcher_->dispatch_(target, WM_NCMOUSELEAVE, 0, 0, result, false);
		else
			dispatcher->dispatch_(target, WM_NCMOUSELEAVE, 0, 0, result, false);
	}

	if (target.handles_message_(WM_NCMOUSELEAVE))
		default_dispatcher_->dispatch_(target, msg, 0, 0, result, true);
	else
		dispatcher->dispatch_(target, msg, 0, 0, result, true);

	return result;
}

LRESULT winp::thread::windows_manager::mouse_move_(ui::window_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	return 0;
}

LRESULT winp::thread::windows_manager::mouse_down_(ui::window_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	return 0;
}

LRESULT winp::thread::windows_manager::mouse_up_(ui::window_surface &target, DWORD mouse_position, WPARAM wparam, LPARAM lparam, bool prevent_default){
	return 0;
}

void winp::thread::windows_manager::init_dispatchers_(){
	default_dispatcher_ = std::make_shared<message::dispatcher>();

	dispatchers_[WM_CREATE] = std::make_shared<message::create_destroy_dispatcher>();
	dispatchers_[WM_DESTROY] = std::make_shared<message::create_destroy_dispatcher>();
}

winp::message::dispatcher *winp::thread::windows_manager::find_dispatcher_(UINT msg){
	auto it = dispatchers_.find(msg);
	return ((it == dispatchers_.end()) ? default_dispatcher_.get() : it->second.get());
}

void winp::thread::windows_manager::track_mouse_(HWND target, UINT flags){
	TRACKMOUSEEVENT info{ sizeof(TRACKMOUSEEVENT), (TME_LEAVE | flags), target, 0 };
	TrackMouseEvent(&info);
}

LRESULT CALLBACK winp::thread::windows_manager::entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam){
	auto &manager = app::object::current_thread_->windows_manager_;

	auto object = manager.find_object_(handle);
	if (object == nullptr)//Forward message
		return ((IsWindowUnicode(handle) == FALSE) ? CallWindowProcA(DefWindowProcA, handle, msg, wparam, lparam) : CallWindowProcW(DefWindowProcW, handle, msg, wparam, lparam));

	switch (msg){
	case WM_NCMOUSEMOVE:
	{
		auto result = CallWindowProcW(object->get_default_message_entry_(), handle, msg, wparam, lparam);

		auto window_ancestor = object->get_first_ancestor_of_<ui::window_surface>();
		if (window_ancestor != nullptr)
			manager.mouse_move_(*window_ancestor, GetMessagePos(), 0, 0, true);

		return result;
	}
	case WM_MOUSEMOVE:
		return manager.mouse_move_(*object, GetMessagePos(), wparam, lparam, false);
	case WM_NCMOUSELEAVE:
	case WM_MOUSELEAVE:
		return manager.mouse_leave_(*object, GetMessagePos(), msg);
	default:
		break;
	}

	LRESULT result = 0;
	if (object->handles_message_(msg))
		default_dispatcher_->dispatch_(*object, msg, wparam, lparam, result, true);
	else
		find_dispatcher_(msg)->dispatch_(*object, msg, wparam, lparam, result, true);

	return result;
}

LRESULT CALLBACK winp::thread::windows_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
	switch (code){
	case HCBT_CREATEWND:
		app::object::current_thread_->windows_manager_.create_window_(reinterpret_cast<HWND>(wparam), *reinterpret_cast<CBT_CREATEWNDW *>(wparam));
		break;
	case HCBT_DESTROYWND:
		app::object::current_thread_->windows_manager_.destroy_window_(reinterpret_cast<HWND>(wparam));
		break;
	default:
		break;
	}

	return CallNextHookEx(nullptr, code, wparam, lparam);
}

std::shared_ptr<winp::message::dispatcher> winp::thread::windows_manager::default_dispatcher_;

std::unordered_map<UINT, std::shared_ptr<winp::message::dispatcher>> winp::thread::windows_manager::dispatchers_;
