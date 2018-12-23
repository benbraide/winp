#include "../app/app_object.h"
#include "button_control.h"

winp::control::button::button(){
	padding_ = m_rect_type{ 5, 2, 5, 2 };
	update_size_();
}

winp::control::button::button(thread::object &thread)
	: object(thread){
	padding_ = m_rect_type{ 5, 2, 5, 2 };
	update_size_();
}

winp::control::button::button(ui::tree &parent)
	: object(parent.get_thread()){
	padding_ = m_rect_type{ 5, 2, 5, 2 };
	update_size_();
	change_parent_(&parent);
}

winp::control::button::~button() = default;

const wchar_t *winp::control::button::get_class_name_() const{
	return WC_BUTTONW;
}

winp::ui::surface::m_size_type winp::control::button::compute_additional_size_(const m_size_type &size) const{
	m_size_type scaled_size{ static_cast<int>(size.cx * scale_.width), static_cast<int>(size.cy * scale_.height) };
	return m_size_type{ (scaled_size.cx - size.cx), (scaled_size.cy - size.cy) };
}
