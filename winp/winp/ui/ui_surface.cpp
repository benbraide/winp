#include "ui_surface.h"

winp::ui::surface::surface(thread::object &thread)
	: tree(thread){
	init_();
}

winp::ui::surface::surface(tree &parent)
	: tree(parent){
	init_();
}

winp::ui::surface::~surface() = default;

void winp::ui::surface::init_(){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
		if (&prop == &visible){
			if (*static_cast<const bool *>(value))
				set_state_(state_visible);
			else
				remove_state_(state_visible);
		}
		else if (&prop == &transparent){
			if (*static_cast<const bool *>(value))
				set_state_(state_transparent);
			else
				remove_state_(state_transparent);
		}
		else if (&prop == &size)
			set_size_(*static_cast<const m_size_type *>(value));
		else if (&prop == &position)
			set_position_(*static_cast<const m_point_type *>(value));
		else if (&prop == &absolute_position)
			set_absolute_position_(*static_cast<const m_point_type *>(value));
		else if (&prop == &color)
			set_color_(*static_cast<const m_rgba_type *>(value));
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &visible)
			*static_cast<bool *>(buf) = has_state_(state_visible);
		else if (&prop == &transparent)
			*static_cast<bool *>(buf) = has_state_(state_transparent);
		else if (&prop == &size)
			*static_cast<m_size_type *>(buf) = get_size_();
		else if (&prop == &position)
			*static_cast<m_point_type *>(buf) = get_position_();
		else if (&prop == &absolute_position)
			*static_cast<m_point_type *>(buf) = get_absolute_position_();
		else if (&prop == &color)
			*static_cast<m_rgba_type *>(buf) = get_color_();
	};

	visible.init_(nullptr, setter, getter);
	transparent.init_(nullptr, setter, getter);

	size.init_(nullptr, setter, getter);
	position.init_(nullptr, setter, getter);
	absolute_position.init_(nullptr, setter, getter);

	color.init_(nullptr, setter, getter);
}

winp::ui::surface *winp::ui::surface::get_surface_parent_() const{
	return dynamic_cast<surface *>(get_parent_());
}

void winp::ui::surface::set_state_(unsigned int value){
	state_ |= value;
}

void winp::ui::surface::remove_state_(unsigned int value){
	state_ &= ~value;
}

bool winp::ui::surface::has_state_(unsigned int value) const{
	return ((state_ & value) == value);
}

void winp::ui::surface::set_size_(const m_size_type &value){
	size_ = value;
}

winp::ui::surface::m_size_type winp::ui::surface::get_size_() const{
	return size_;
}

void winp::ui::surface::set_position_(const m_point_type &value){
	position_ = value;
}

winp::ui::surface::m_point_type winp::ui::surface::get_position_() const{
	return position_;
}

void winp::ui::surface::set_absolute_position_(const m_point_type &value){
	auto surface_parent = get_surface_parent_();
	set_position_((surface_parent == nullptr) ? value : surface_parent->convert_position_from_absolute_value_(value));
}

winp::ui::surface::m_point_type winp::ui::surface::get_absolute_position_() const{
	auto surface_parent = get_surface_parent_();
	return ((surface_parent == nullptr) ? get_position_() : surface_parent->convert_position_to_absolute_value_(get_position_()));
}

void winp::ui::surface::set_color_(const m_rgba_type &value){
	color_ = value;
}

winp::ui::surface::m_rgba_type winp::ui::surface::get_color_() const{
	return color_;
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_from_absolute_value_(const m_point_type &value) const{
	auto my_absolute_position = get_absolute_position_();
	return m_point_type{ (value.x - my_absolute_position.x), (value.y - my_absolute_position.y) };
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_to_absolute_value_(const m_point_type &value) const{
	auto my_absolute_position = get_absolute_position_();
	return m_point_type{ (my_absolute_position.x + value.x), (my_absolute_position.y + value.y) };
}
