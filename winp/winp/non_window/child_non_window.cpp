#include "../app/app_object.h"

winp::non_window::child::child(ui::window_surface &parent)
	: visible_surface(parent.get_thread()){
	change_parent_(&parent);
}

winp::non_window::child::child(child &parent)
	: visible_surface(parent.get_thread()){
	change_parent_(&parent);
}

winp::non_window::child::~child(){
	destruct();
}

bool winp::non_window::child::set_border_type(border_type value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_border_type_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_border_type_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::non_window::child::border_type winp::non_window::child::get_border_type(const std::function<void(border_type)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_border_type_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_border_type_()); }, thread::queue::send_priority, id_);
		return border_type::nil;
	}

	return thread_.queue.execute([this]{ return get_border_type_(); }, thread::queue::send_priority, id_);
}

bool winp::non_window::child::set_border_curve_size(const m_size_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_border_curve_size_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_border_curve_size_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::ui::surface::m_size_type winp::non_window::child::get_border_curve_size(const std::function<void(const m_size_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_border_curve_size_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_border_curve_size_()); }, thread::queue::send_priority, id_);
		return m_size_type{};
	}

	return thread_.queue.execute([this]{ return get_border_curve_size_(); }, thread::queue::send_priority, id_);
}

void winp::non_window::child::destruct_(){
	destroy_();
	visible_surface::destruct_();
}

bool winp::non_window::child::create_(){
	if (get_handle_() != nullptr)
		return true;//Already created

	HRGN handle = nullptr;
	auto region = get_dimension_();
	auto &border_curve_size = get_border_curve_size_();

	switch (get_border_type_()){
	case border_type::rect:
		handle = CreateRectRgnIndirect(&region);
		break;
	case border_type::round_rect:
		handle = CreateRoundRectRgn(region.left, region.top, region.right, region.bottom, border_curve_size.cx, border_curve_size.cy);
		break;
	case border_type::ellipse:
		handle = CreateEllipticRgnIndirect(&region);
		break;
	default:
		break;
	}

	if (handle != nullptr){
		set_handle_(handle);
		redraw_(m_rect_type{});
	}

	return (get_handle_() != nullptr);
}

bool winp::non_window::child::destroy_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return true;

	hide_region_(m_rect_type{});
	if (DeleteObject(handle) != FALSE)
		set_handle_(nullptr);

	return (get_handle_() == nullptr);
}

bool winp::non_window::child::set_size_(const m_size_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return visible_surface::set_size_(value);

	auto size = get_size_();
	if (value.cx == size.cx && value.cy == size.cy)
		return true;//No changes

	auto position = get_position_();
	m_rect_type updated_region{
		position.x,
		position.y,
		(position.x + value.cx),
		(position.y + value.cy)
	};

	if (border_type_ == border_type::rect)
		hide_region_(updated_region);
	else//Region object cannot be resized
		destroy_();

	size_ = value;//Update value
	if (border_type_ == border_type::rect)
		SetRectRgn(static_cast<HRGN>(handle), position.x, position.y, (position.x + value.cx), (position.y + value.cy));
	else//Region object cannot be resized
		create_();

	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::set_position_(const m_point_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return visible_surface::set_position_(value);

	auto position = get_position_();
	if (value.x == position.x && value.y == position.y)
		return true;//No changes

	auto size = get_size_();
	m_rect_type updated_region{
		value.x,
		value.y,
		(value.x + size.cx),
		(value.y + size.cy)
	};

	hide_region_(updated_region);
	OffsetRgn(static_cast<HRGN>(handle), value.x, value.y);
	redraw_(m_rect_type{});

	return true;
}

UINT winp::non_window::child::hit_test_(const m_point_type &pt, bool is_absolute) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return visible_surface::hit_test_(pt, is_absolute);

	auto relative_pt = (is_absolute ? convert_position_from_absolute_value_(pt) : pt);
	return (PtInRegion(static_cast<HRGN>(handle), relative_pt.x, relative_pt.y) != FALSE);
}

void winp::non_window::child::redraw_(const m_rect_type &region){
	if (!is_visible_() || get_handle_() == nullptr)
		return;

	auto parent = get_first_ancestor_of_<ui::visible_surface>();
	if (parent == nullptr)
		return;

	auto position = get_position_();
	auto update_region = ((region.left < region.right && region.top < region.bottom) ? region : get_client_dimension_());

	OffsetRect(&update_region, position.x, position.y);
	parent->redraw_(update_region);
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

void winp::non_window::child::hide_region_(m_rect_type region){
	if (!is_visible_() || get_handle_() == nullptr)
		return;

	m_rect_type client_rect = get_client_dimension_(), computed{};
	region = ((region.left < region.right && region.top < region.bottom) ? region : client_rect);
	IntersectRect(&computed, &client_rect, &region);

	auto was_visible = is_visible_();
	if (was_visible)//Make invisible
		state_ &= ~state_visible;

	if (IsRectEmpty(&computed) == FALSE){
		if (computed.left != client_rect.left || computed.top != client_rect.top || computed.right != client_rect.right || computed.bottom != client_rect.bottom){//Region is shrunken
			SubtractRect(&computed, &client_rect, &computed);
			redraw_(computed);
		}
	}
	else//Region lies outside
		redraw_(m_rect_type{});

	if (was_visible)//Restore visibility
		state_ |= state_visible;
}

bool winp::non_window::child::set_border_type_(border_type value){
	if (value == get_border_type_())
		return true;

	border_type_ = value;
	if (auto handle = get_handle_(); get_handle_() != nullptr){//Update view
		if (border_type_ != border_type::rect){
			auto region = get_dimension_();
			if (SetRectRgn(static_cast<HRGN>(handle), region.left, region.top, region.right, region.bottom) != FALSE)
				redraw_(m_rect_type{});
		}
		else{//Recreate
			destroy_();
			create_();
		}
	}

	return true;
}

winp::non_window::child::border_type winp::non_window::child::get_border_type_() const{
	return border_type_;
}

bool winp::non_window::child::set_border_curve_size_(const m_size_type &value){
	if (value.cx == border_curve_size_.cx && value.cy == border_curve_size_.cy)
		return true;//No changes

	border_curve_size_ = value;
	if (get_border_type_() == border_type::round_rect && get_handle_() != nullptr){//Recreate
		destroy_();
		create_();
	}

	return true;
}

const winp::ui::surface::m_size_type &winp::non_window::child::get_border_curve_size_() const{
	return border_curve_size_;
}
