#include "ui_visible_surface.h"

winp::ui::visible_surface::visible_surface(thread::object &thread)
	: surface(thread), background_color_(0){
	show_event.thread_ = thread_;
	hide_event.thread_ = thread_;
	draw_event.thread_ = thread_;

	auto sys_color = GetSysColor(COLOR_WINDOW);
	background_color_ = D2D1::ColorF(
		(GetRValue(sys_color) / 255.0f),
		(GetGValue(sys_color) / 255.0f),
		(GetBValue(sys_color) / 255.0f)
	);
}

winp::ui::visible_surface::~visible_surface() = default;

void winp::ui::visible_surface::show(const std::function<void(object &, bool)> &callback){
	show(0, callback);
}

void winp::ui::visible_surface::show(int how, const std::function<void(object &, bool)> &callback){
	set_visibility(true, callback);
}

void winp::ui::visible_surface::hide(const std::function<void(object &, bool)> &callback){
	set_visibility(false, callback);
}

void winp::ui::visible_surface::set_visibility(bool is_visible, const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = set_visibility_(is_visible);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority);
}

bool winp::ui::visible_surface::is_visible(const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(is_visible_()); }, thread::queue::send_priority);
		return false;
	}

	return thread_->queue.add([this]{ return is_visible_(); }, thread::queue::send_priority).get();
}

void winp::ui::visible_surface::set_transparency(bool is_transparent, const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = set_transparency_(is_transparent);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority);
}

bool winp::ui::visible_surface::is_transparent(const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(is_transparent_()); }, thread::queue::send_priority);
		return false;
	}

	return thread_->queue.add([this]{ return is_transparent_(); }, thread::queue::send_priority).get();
}

void winp::ui::visible_surface::set_background_color(const D2D1::ColorF &value, const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = set_background_color_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority);
}

D2D1::ColorF winp::ui::visible_surface::get_background_color(const std::function<void(const D2D1::ColorF &)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(get_background_color_()); }, thread::queue::send_priority);
		return D2D1::ColorF(0);
	}

	return *(thread_->queue.add([this]{ return &get_background_color_(); }, thread::queue::send_priority).get());
}

winp::ui::visible_surface::m_colorf winp::ui::visible_surface::convert_from_d2d1_colorf(const D2D1::ColorF &value){
	return m_colorf{ value.r, value.g, value.b, value.a };
}

D2D1::ColorF winp::ui::visible_surface::convert_to_d2d1_colorf(const m_colorf &value){
	return D2D1::ColorF(value.r, value.g, value.b, value.a);
}

winp::ui::visible_surface *winp::ui::visible_surface::get_visible_surface_parent_() const{
	return dynamic_cast<visible_surface *>(get_parent_());
}

/*
void winp::ui::visible_surface::toggle_state_(unsigned int value, bool set){
	if (set)
		set_state_(value);
	else
		remove_state_(value);
}

void winp::ui::visible_surface::set_state_(unsigned int value){
	state_ |= value;
}

void winp::ui::visible_surface::remove_state_(unsigned int value){
	state_ &= ~value;
}

bool winp::ui::visible_surface::has_state_(unsigned int value) const{
	return ((state_ & value) == value);
}*/

void winp::ui::visible_surface::redraw_(){}

bool winp::ui::visible_surface::set_visibility_(bool is_visible){
	//toggle_state_(state_visible, state);
	return false;
}

bool winp::ui::visible_surface::is_visible_() const{
	//return has_state_(state_visible);
	return false;
}

bool winp::ui::visible_surface::set_transparency_(bool is_transparent){
	//toggle_state_(state_transparent, state);
	return false;
}

bool winp::ui::visible_surface::is_transparent_() const{
	//return has_state_(state_transparent);
	return false;
}

bool winp::ui::visible_surface::set_background_color_(const D2D1::ColorF &value){
	background_color_ = value;
	redraw_();
	return true;
}

const D2D1::ColorF &winp::ui::visible_surface::get_background_color_() const{
	return background_color_;
}
