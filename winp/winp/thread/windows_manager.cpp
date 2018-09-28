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

void winp::thread::windows_manager::init_dispatchers_(){
	default_dispatcher_ = std::make_shared<message::dispatcher>();

	dispatchers_[WM_CREATE] = std::make_shared<message::create_destroy_dispatcher>();
	dispatchers_[WM_DESTROY] = std::make_shared<message::create_destroy_dispatcher>();
}

winp::message::dispatcher *winp::thread::windows_manager::find_dispatcher_(UINT msg){
	auto it = dispatchers_.find(msg);
	return ((it == dispatchers_.end()) ? default_dispatcher_.get() : it->second.get());
}

LRESULT CALLBACK winp::thread::windows_manager::entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam){
	auto object = app::object::current_thread_->windows_manager_.find_object_(handle);
	if (object == nullptr)//Forward message
		return ((IsWindowUnicode(handle) == FALSE) ? CallWindowProcA(DefWindowProcA, handle, msg, wparam, lparam) : CallWindowProcW(DefWindowProcW, handle, msg, wparam, lparam));

	LRESULT result = 0;
	if (!find_dispatcher_(msg)->dispatch_(*object, msg, wparam, lparam, result))//Default not prevented
		result = (object->get_default_message_entry_())(handle, msg, wparam, lparam);

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
