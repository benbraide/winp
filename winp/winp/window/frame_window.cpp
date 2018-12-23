#include "frame_window.h"

winp::window::frame::frame(){
	styles_ = WS_OVERLAPPEDWINDOW;
}

winp::window::frame::frame(thread::object &thread)
	: window_surface(thread), system_menu_(thread){
	styles_ = WS_OVERLAPPEDWINDOW;
}

winp::window::frame::~frame() = default;

bool winp::window::frame::set_caption(const std::wstring &value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_caption_(value));
	});
}

std::wstring winp::window::frame::get_caption(const std::function<void(const std::wstring &)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_caption_());
	}, callback != nullptr);
}

winp::menu::wrapper_collection &winp::window::frame::get_system_menu(const std::function<void(menu::wrapper_collection &)> &callback){
	if (callback != nullptr)
		callback(system_menu_);
	return system_menu_;
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
