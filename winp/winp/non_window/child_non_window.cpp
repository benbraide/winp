#include "../app/app_object.h"

winp::non_window::child::child(ui::io_surface &parent)
	: io_surface(parent.get_thread()){
	change_parent_(&parent);
}

winp::non_window::child::~child() = default;

void winp::non_window::child::redraw_(const m_rect_type &region){
	if (!is_visible_())
		return;

	m_point_type offset;
	auto reg = ((region.left < region.right || region.top < region.bottom) ? region : get_dimension_());

	ui::window_surface *window_parent = nullptr;
	for (auto parent = get_surface_parent_(); parent != nullptr && (window_parent = dynamic_cast<ui::window_surface *>(parent)) == nullptr; parent = get_surface_parent_()){
		offset = parent->get_position_();
		reg.left += offset.x;
		reg.top += offset.y;
		reg.right += offset.x;
		reg.bottom += offset.y;
	}

	if (window_parent != nullptr)
		static_cast<ui::visible_surface *>(window_parent)->redraw_(reg);
}

bool winp::non_window::child::set_visibility_(bool is_visible){
	if (is_visible_() == is_visible)
		return true;

	if (is_visible)
		state_ |= state_visible;
	else
		state_ &= ~state_visible;

	redraw_(m_rect_type{});
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

	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::is_transparent_() const{
	return ((state_ & state_transparent) == state_transparent);
}

bool winp::non_window::child::handle_parent_change_event_(event::tree &e){
	return (e.get_attached_parent() != nullptr);
}

void winp::non_window::child::handle_parent_changed_event_(event::tree &e){
	if (!is_visible_())
		return;

	m_point_type offset;
	auto reg = get_dimension_();

	ui::window_surface *window_parent = nullptr;
	for (auto parent = dynamic_cast<ui::surface *>(e.get_attached_parent()); parent != nullptr && (window_parent = dynamic_cast<ui::window_surface *>(parent)) == nullptr; parent = get_surface_parent_()){
		offset = parent->get_position_();
		reg.left += offset.x;
		reg.top += offset.y;
		reg.right += offset.x;
		reg.bottom += offset.y;
	}

	if (window_parent != nullptr)//Redraw previous parent
		static_cast<ui::visible_surface *>(window_parent)->redraw_(reg);

	redraw_(m_rect_type{});
}
