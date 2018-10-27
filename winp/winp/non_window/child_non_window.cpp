#include "child_non_window.h"

winp::non_window::child::child(ui::io_surface &parent)
	: io_surface(parent.get_thread()){
	set_parent(&parent);
}

winp::non_window::child::~child() = default;

bool winp::non_window::child::validate_parent_change_(tree *value, std::size_t index) const{
	return (value != nullptr && get_parent_() == nullptr/* && value->*/);
}

void winp::non_window::child::parent_changed_(tree *previous_parent, std::size_t previous_index){
	io_surface::parent_changed_(previous_parent, previous_index);
	redraw_();
}

void winp::non_window::child::redraw_(){

}

bool winp::non_window::child::set_visibility_(bool is_visible){
	if (is_visible_() == is_visible)
		return true;

	if (is_visible)
		state_ |= state_visible;
	else
		state_ &= ~state_visible;

	redraw_();
	return true;
}

bool winp::non_window::child::is_visible_() const{
	return ((state_ & state_visible) == state_visible);
}

bool winp::non_window::child::set_transparency_(bool is_transparent){
	if (is_transparent_() == is_transparent)
		return true;

	if (is_transparent)
		state_ |= state_transparent;
	else
		state_ &= ~state_transparent;

	redraw_();
	return true;
}

bool winp::non_window::child::is_transparent_() const{
	return ((state_ & state_transparent) == state_transparent);
}
