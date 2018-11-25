#include "frame_window.h"

winp::window::frame::frame(){
	styles_ = WS_OVERLAPPEDWINDOW;
}

winp::window::frame::frame(thread::object &thread)
	: window_surface(thread), system_menu_(thread){
	styles_ = WS_OVERLAPPEDWINDOW;
}

winp::window::frame::~frame() = default;

bool winp::window::frame::set_caption(const std::wstring &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_caption_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_caption_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

std::wstring winp::window::frame::get_caption(const std::function<void(const std::wstring &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_caption_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_caption_()); }, thread::queue::send_priority, id_);
		return L"";
	}

	return thread_.queue.execute([=]{ return get_caption_(); }, thread::queue::send_priority, id_);
}

winp::menu::wrapper_collection *winp::window::frame::get_system_menu(const std::function<void(menu::wrapper_collection &)> &callback){
	if (thread_.is_thread_context()){
		auto result = get_system_menu_();
		if (callback != nullptr)
			callback(*result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(*get_system_menu_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([=]{ return get_system_menu_(); }, thread::queue::send_priority, id_);
}

DWORD winp::window::frame::get_persistent_styles_() const{
	return (window_surface::get_persistent_styles_() | (WS_CAPTION | WS_THICKFRAME));
}

DWORD winp::window::frame::get_filtered_styles_() const{
	return (window_surface::get_filtered_styles_() | (WS_CAPTION | WS_THICKFRAME));
}

const wchar_t *winp::window::frame::get_window_text_() const{
	return caption_.data();
}

bool winp::window::frame::set_caption_(const std::wstring &value){
	auto handle = get_handle_();
	if (handle != nullptr)
		return (SendMessageW(static_cast<HWND>(handle), WM_SETTEXT, 0, reinterpret_cast<WPARAM>(value.data())) != FALSE);

	caption_ = value;
	return true;
}

const std::wstring &winp::window::frame::get_caption_() const{
	return caption_;
}

winp::menu::wrapper_collection *winp::window::frame::get_system_menu_(){
	return &system_menu_;
}
