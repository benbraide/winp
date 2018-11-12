#include "frame_window.h"

winp::window::frame::frame(){
	styles_ = WS_OVERLAPPEDWINDOW;
}

winp::window::frame::frame(thread::object &thread)
	: window_surface(thread){
	styles_ = WS_OVERLAPPEDWINDOW;
}

winp::window::frame::~frame(){
	destruct_();
}

void winp::window::frame::set_caption(const std::wstring &value, const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = set_caption_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

std::wstring winp::window::frame::get_caption(const std::function<void(const std::wstring &)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(get_caption_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_->queue.add([=]{ return get_caption_(); }, thread::queue::send_priority, id_).get();
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
		return (SendMessageW(handle, WM_SETTEXT, 0, reinterpret_cast<WPARAM>(value.data())) != FALSE);

	caption_ = value;
	return true;
}

const std::wstring &winp::window::frame::get_caption_() const{
	return caption_;
}
