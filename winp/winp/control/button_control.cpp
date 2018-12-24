#include "../app/app_object.h"
#include "button_control.h"

winp::control::button::button(){
	padding_ = m_rect_type{ 10, 5, 10, 5 };
	update_size_();
}

winp::control::button::button(thread::object &thread)
	: object(thread){
	padding_ = m_rect_type{ 10, 5, 10, 5 };
	update_size_();
}

winp::control::button::button(ui::tree &parent)
	: object(parent.get_thread()){
	padding_ = m_rect_type{ 10, 5, 10, 5 };
	update_size_();
	change_parent_(&parent);
}

winp::control::button::~button() = default;

DWORD winp::control::button::get_persistent_styles_() const{
	return (object::get_persistent_styles_() | (is_uniform_padding_() ? 0u : BS_OWNERDRAW));
}

DWORD winp::control::button::get_filtered_styles_() const{
	return (object::get_filtered_styles_() | BS_OWNERDRAW);
}

const wchar_t *winp::control::button::get_class_name_() const{
	return WC_BUTTONW;
}

void winp::control::button::padding_changed_(){
	object::padding_changed_();
	update_styles_(false);
}

winp::ui::surface::m_size_type winp::control::button::compute_additional_size_(const m_size_type &size) const{
	m_size_type scaled_size{ static_cast<int>(size.cx * scale_.width), static_cast<int>(size.cy * scale_.height) };
	return m_size_type{ (scaled_size.cx - size.cx), (scaled_size.cy - size.cy) };
}
