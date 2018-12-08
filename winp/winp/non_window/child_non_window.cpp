#include "../app/app_object.h"

winp::non_window::child::child(thread::object &thread)
	: visible_surface(thread){}

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

bool winp::non_window::child::set_client_border_type(border_type value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_client_border_type_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_client_border_type_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::non_window::child::border_type winp::non_window::child::get_client_border_type(const std::function<void(border_type)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_client_border_type_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_client_border_type_()); }, thread::queue::send_priority, id_);
		return border_type::nil;
	}

	return thread_.queue.execute([this]{ return get_client_border_type_(); }, thread::queue::send_priority, id_);
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

bool winp::non_window::child::set_border_curve_size(int width, int height, const std::function<void(object &, bool)> &callback){
	return set_border_curve_size(m_size_type{ width, height }, callback);
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

bool winp::non_window::child::set_client_border_curve_size(const m_size_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_client_border_curve_size_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_client_border_curve_size_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::non_window::child::set_client_border_curve_size(int width, int height, const std::function<void(object &, bool)> &callback){
	return set_client_border_curve_size(m_size_type{ width, height }, callback);
}

winp::ui::surface::m_size_type winp::non_window::child::get_client_border_curve_size(const std::function<void(const m_size_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_client_border_curve_size_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_client_border_curve_size_()); }, thread::queue::send_priority, id_);
		return m_size_type{};
	}

	return thread_.queue.execute([this]{ return get_client_border_curve_size_(); }, thread::queue::send_priority, id_);
}

void winp::non_window::child::destruct_(){
	destroy_();
	visible_surface::destruct_();
}

bool winp::non_window::child::create_(){
	if (get_handle_() != nullptr)
		return true;//Already created

	if (auto non_window_parent = get_first_ancestor_of_<child, ui::window_surface>(); non_window_parent != nullptr && non_window_parent->get_handle_() == nullptr)
		return false;//Parent not created
	
	if (auto window_parent = get_first_ancestor_of_<ui::window_surface, child>(); window_parent != nullptr && window_parent->get_handle_() == nullptr)
		return false;//Parent not created

	HRGN handle = nullptr;
	auto &border_curve_size = get_border_curve_size_(), &client_border_curve_size = get_client_border_curve_size_();

	auto region = get_dimension_();
	OffsetRect(&region, -region.left, -region.top);//Move to (0, 0)

	auto border_type = get_border_type_(), client_border_type = get_client_border_type_();
	switch (border_type){
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
		handle = create_complex_region_();
		break;
	}

	if (handle != nullptr){
		set_handle_(handle);
		create_client_region_();
		if (client_region_ == nullptr)
			redraw_(m_rect_type{});

		dispatch_message_(WM_NCCREATE, 0, 0);
		if (auto parent = get_parent_(); parent != nullptr)
			parent->call_hook_(ui::hook::child_size_change_hook_code);

		for (auto child : children_)
			child->call_hook_(ui::hook::parent_size_change_hook_code);
	}

	return (get_handle_() != nullptr);
}

bool winp::non_window::child::destroy_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return true;

	if (DeleteObject(handle) == FALSE)
		return false;

	if (client_region_ != nullptr){
		if (client_region_ != handle)
			DeleteObject(client_region_);
		client_region_ = nullptr;
	}

	set_handle_(nullptr);
	dispatch_message_(WM_NCDESTROY, 0, 0);

	if (auto parent = get_first_ancestor_of_<ui::visible_surface>(); parent != nullptr && is_visible_())
		parent->redraw_(get_dimension_());//Clear view

	return (get_handle_() == nullptr);
}

bool winp::non_window::child::set_size_(const m_size_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return visible_surface::set_size_(value);

	auto size = get_size_();
	if (value.cx == size.cx && value.cy == size.cy)
		return true;//No changes

	if (border_type_ == border_type::rect)
		redraw_(m_rect_type{});
	else//Region object cannot be resized
		destroy_();

	if (!visible_surface::set_size_(value))
		return false;

	if (border_type_ == border_type::rect){
		SetRectRgn(static_cast<HRGN>(handle), 0, 0, value.cx, value.cy);
		redraw_(m_rect_type{});
	}
	else//Region object cannot be resized
		create_();

	return true;
}

bool winp::non_window::child::set_position_(const m_point_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return visible_surface::set_position_(value);

	auto position = get_position_();
	if (value.x == position.x && value.y == position.y)
		return true;//No changes

	redraw_(m_rect_type{});
	if (!visible_surface::set_position_(value))
		return false;

	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::set_padding_(const m_rect_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return visible_surface::set_padding_(value);

	auto padding = get_padding_();
	if (value.left == padding.left && value.top == padding.top && value.right == padding.right && value.bottom == padding.bottom)
		return true;//No changes

	redraw_(m_rect_type{});
	if (!visible_surface::set_padding_(value))
		return false;

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
	auto update_region = ((IsRectEmpty(&region) == FALSE) ? region : get_client_dimension_());

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
		state_ |= (state_transparent | state_transparent_client);
	else
		state_ &= ~(state_transparent | state_transparent_client);

	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::is_transparent_() const{
	return ((state_ & state_transparent) == state_transparent);
}

bool winp::non_window::child::set_client_transparency_(bool is_transparent){
	if (is_transparent_client_() == is_transparent)
		return true;

	if (is_transparent)
		state_ |= state_transparent_client;
	else
		state_ &= ~state_transparent_client;

	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::is_transparent_client_() const{
	return ((state_ & state_transparent_client) == state_transparent_client);
}

bool winp::non_window::child::create_client_region_(){
	auto &padding = get_padding_();
	auto no_padding = (padding.left == 0 && padding.top == 0 && padding.right == 0 && padding.bottom == 0);

	auto border_type = get_client_border_type_();
	auto &border_curve_size = get_border_curve_size_(), &client_border_curve_size = get_client_border_curve_size_();

	auto is_complex = (border_type == border_type::complex);
	auto is_same_border_curve_size = (!is_complex && border_type == get_border_type_() && border_curve_size.cx == client_border_curve_size.cx && border_curve_size.cy == client_border_curve_size.cy);

	if (no_padding && !is_complex && is_same_border_curve_size)
		return true;//Same structure

	auto region = get_client_dimension_();
	OffsetRect(&region, -region.left, -region.top);//Move to (0, 0)

	switch (border_type){
	case border_type::rect:
		client_region_ = CreateRectRgnIndirect(&region);
		break;
	case border_type::round_rect:
		client_region_ = CreateRoundRectRgn(region.left, region.top, region.right, region.bottom, client_border_curve_size.cx, client_border_curve_size.cy);
		break;
	case border_type::ellipse:
		client_region_ = CreateEllipticRgnIndirect(&region);
		break;
	default:
		client_region_ = create_complex_client_region_();
		break;
	}

	if (client_region_ != nullptr){
		redraw_(m_rect_type{});
		return true;
	}

	return false;
}

bool winp::non_window::child::set_border_type_(border_type value){
	if (value == get_border_type_())
		return true;

	border_type_ = value;
	if (auto handle = get_handle_(); handle != nullptr){//Update view
		if (border_type_ == border_type::rect){
			auto region = get_dimension_();
			OffsetRect(&region, -region.left, -region.top);//Move to (0, 0)

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

bool winp::non_window::child::set_client_border_type_(border_type value){
	if (value == get_client_border_type_())
		return true;

	client_border_type_ = value;
	if (get_handle_() != nullptr){//Update view
		if (client_border_type_ == border_type::rect){
			auto region = get_client_dimension_();
			OffsetRect(&region, -region.left, -region.top);//Move to (0, 0)

			if (SetRectRgn(client_region_, region.left, region.top, region.right, region.bottom) != FALSE)
				redraw_(m_rect_type{});
		}
		else if (client_region_ == nullptr || DeleteObject(client_region_) != FALSE)
			create_client_region_();
	}

	return true;
}

winp::non_window::child::border_type winp::non_window::child::get_client_border_type_() const{
	return client_border_type_;
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

bool winp::non_window::child::set_client_border_curve_size_(const m_size_type &value){
	if (value.cx == client_border_curve_size_.cx && value.cy == client_border_curve_size_.cy)
		return true;//No changes

	client_border_curve_size_ = value;
	if (get_client_border_type_() == border_type::round_rect && get_handle_() != nullptr && (client_region_ == nullptr || DeleteObject(client_region_) != FALSE))
		create_client_region_();

	return true;
}

const winp::ui::surface::m_size_type &winp::non_window::child::get_client_border_curve_size_() const{
	return client_border_curve_size_;
}

HRGN winp::non_window::child::create_complex_region_() const{
	return nullptr;
}

HRGN winp::non_window::child::create_complex_client_region_() const{
	return nullptr;
}
