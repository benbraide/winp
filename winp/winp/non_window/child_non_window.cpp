#include "../app/app_object.h"

winp::non_window::child::child(thread::object &thread)
	: visible_surface(thread){
	non_client_background_color_ = background_color_;
}

winp::non_window::child::child(ui::window_surface &parent)
	: visible_surface(parent.get_thread()){
	non_client_background_color_ = background_color_;
	change_parent_(&parent);
}

winp::non_window::child::child(child &parent)
	: visible_surface(parent.get_thread()){
	non_client_background_color_ = background_color_;
	change_parent_(&parent);
}

winp::non_window::child::~child(){
	destruct();
}

bool winp::non_window::child::set_non_client_transparency(bool is_transparent, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_non_client_transparency_(is_transparent);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_non_client_transparency_(is_transparent);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::non_window::child::is_transparent_non_client(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_transparent_non_client_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_transparent_non_client_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_transparent_non_client_(); }, thread::queue::send_priority, id_);
}

bool winp::non_window::child::set_client_transparency(bool is_transparent, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_client_transparency_(is_transparent);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_client_transparency_(is_transparent);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::non_window::child::is_transparent_client(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_transparent_client_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_transparent_client_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_transparent_client_(); }, thread::queue::send_priority, id_);
}

bool winp::non_window::child::set_non_client_background_color(const D2D1::ColorF &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_non_client_background_color_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_non_client_background_color_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

D2D1::ColorF winp::non_window::child::get_non_client_background_color(const std::function<void(const D2D1::ColorF &)> &callback) const{
	if (thread_.is_thread_context()){
		auto &result = get_non_client_background_color_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_non_client_background_color_()); }, thread::queue::send_priority, id_);
		return D2D1::ColorF(0);
	}

	return *(thread_.queue.execute([this]{ return &get_non_client_background_color_(); }, thread::queue::send_priority, id_));
}

bool winp::non_window::child::set_client_background_color(const D2D1::ColorF &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_client_background_color_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_client_background_color_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

D2D1::ColorF winp::non_window::child::get_client_background_color(const std::function<void(const D2D1::ColorF &)> &callback) const{
	if (thread_.is_thread_context()){
		auto &result = get_client_background_color_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_client_background_color_()); }, thread::queue::send_priority, id_);
		return D2D1::ColorF(0);
	}

	return *(thread_.queue.execute([this]{ return &get_client_background_color_(); }, thread::queue::send_priority, id_));
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

bool winp::non_window::child::set_non_client_border_type(border_type value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_non_client_border_type_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_non_client_border_type_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::non_window::child::border_type winp::non_window::child::get_non_client_border_type(const std::function<void(border_type)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_non_client_border_type_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_non_client_border_type_()); }, thread::queue::send_priority, id_);
		return border_type::nil;
	}

	return thread_.queue.execute([this]{ return get_non_client_border_type_(); }, thread::queue::send_priority, id_);
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

bool winp::non_window::child::set_non_client_border_curve_size(const m_size_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_non_client_border_curve_size_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_non_client_border_curve_size_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::non_window::child::set_non_client_border_curve_size(int width, int height, const std::function<void(object &, bool)> &callback){
	return set_non_client_border_curve_size(m_size_type{ width, height }, callback);
}

winp::ui::surface::m_size_type winp::non_window::child::get_non_client_border_curve_size(const std::function<void(const m_size_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_non_client_border_curve_size_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_non_client_border_curve_size_()); }, thread::queue::send_priority, id_);
		return m_size_type{};
	}

	return thread_.queue.execute([this]{ return get_non_client_border_curve_size_(); }, thread::queue::send_priority, id_);
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
	if (auto non_window_parent = get_first_ancestor_of_<child, ui::window_surface>(); non_window_parent != nullptr && non_window_parent->get_handle_() == nullptr)
		return false;//Parent not created
	
	if (auto window_parent = get_first_ancestor_of_<ui::window_surface, child>(); window_parent != nullptr && window_parent->get_handle_() == nullptr)
		return false;//Parent not created

	if (!create_non_client_() || !create_client_()){
		destroy_client_();
		destroy_non_client_();
		return false;
	}

	redraw_(m_rect_type{});
	dispatch_message_(WM_NCCREATE, 0, 0);

	if (auto parent = get_parent_(); parent != nullptr)
		parent->call_hook_(ui::hook::child_size_change_hook_code);

	for (auto child : children_)
		child->call_hook_(ui::hook::parent_size_change_hook_code);

	return true;
}

bool winp::non_window::child::destroy_(){
	if (!destroy_client_() || !destroy_non_client_())
		return false;
	
	if (auto parent = get_first_ancestor_of_<ui::visible_surface>(); parent != nullptr && is_visible_())
		parent->redraw_(get_dimension_());//Clear view

	if (auto parent = get_parent_(); parent != nullptr)
		parent->call_hook_(ui::hook::child_size_change_hook_code);

	return (get_handle_() == nullptr);
}

bool winp::non_window::child::is_created_() const{
	return (get_handle_() != nullptr);
}

bool winp::non_window::child::set_size_(const m_size_type &value){
	if (get_handle_() == nullptr)
		return visible_surface::set_size_(value);

	auto size = get_size_();
	if (value.cx == size.cx && value.cy == size.cy)
		return true;//No changes

	redraw_(m_rect_type{});
	if (!visible_surface::set_size_(value))
		return false;

	if (non_client_handle_ != nullptr && (!destroy_non_client_() || !create_non_client_()))
		return false;

	if (destroy_client_() && create_client_()){
		redraw_(m_rect_type{});
		return true;
	}

	return false;
}

bool winp::non_window::child::set_position_(const m_point_type &value){
	if (get_handle_() == nullptr)
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
	if (get_handle_() == nullptr)
		return visible_surface::set_padding_(value);

	auto padding = get_padding_();
	if (value.left == padding.left && value.top == padding.top && value.right == padding.right && value.bottom == padding.bottom)
		return true;//No changes

	redraw_(m_rect_type{});
	if (!visible_surface::set_padding_(value))
		return false;

	return border_details_changed_(changed_border_type::both);
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
		state_ |= (state_transparent_non_client | state_transparent_client);
	else
		state_ &= ~(state_transparent_non_client | state_transparent_client);

	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::is_transparent_() const{
	return ((state_ & (state_transparent_non_client | state_transparent_client)) == (state_transparent_non_client | state_transparent_client));
}

bool winp::non_window::child::set_background_color_(const D2D1::ColorF &value){
	background_color_ = non_client_background_color_ = value;
	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::set_non_client_transparency_(bool is_transparent){
	if (is_transparent_non_client_() == is_transparent)
		return true;

	if (is_transparent)
		state_ |= state_transparent_non_client;
	else
		state_ &= ~state_transparent_non_client;

	redraw_(m_rect_type{});
	return true;
}

bool winp::non_window::child::is_transparent_non_client_() const{
	return ((state_ & state_transparent_non_client) == state_transparent_non_client);
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

bool winp::non_window::child::set_non_client_background_color_(const D2D1::ColorF &value){
	non_client_background_color_ = value;
	redraw_(m_rect_type{});
	return true;
}

const D2D1::ColorF &winp::non_window::child::get_non_client_background_color_() const{
	return non_client_background_color_;
}

bool winp::non_window::child::set_client_background_color_(const D2D1::ColorF &value){
	background_color_ = value;
	redraw_(m_rect_type{});
	return true;
}

const D2D1::ColorF &winp::non_window::child::get_client_background_color_() const{
	return background_color_;
}

bool winp::non_window::child::create_non_client_(){
	if (non_client_handle_ != nullptr || !should_create_non_client_region_())
		return true;//Already created

	auto &border_curve_size = get_non_client_border_curve_size_();
	auto region = get_dimension_();

	OffsetRect(&region, -region.left, -region.top);//Move to (0, 0)
	switch (get_non_client_border_type_()){
	case border_type::round_rect:
		non_client_handle_ = CreateRoundRectRgn(region.left, region.top, region.right, region.bottom, border_curve_size.cx, border_curve_size.cy);
		break;
	case border_type::ellipse:
		non_client_handle_ = CreateEllipticRgnIndirect(&region);
		break;
	case border_type::complex:
		non_client_handle_ = create_complex_client_region_();
		break;
	default:
		non_client_handle_ = CreateRectRgnIndirect(&region);
		break;
	}

	return (non_client_handle_ != nullptr);
}

bool winp::non_window::child::create_client_(){
	if (get_handle_() != nullptr)
		return true;//Already created

	auto &border_curve_size = get_client_border_curve_size_();
	auto region = get_client_dimension_();

	OffsetRect(&region, -region.left, -region.top);//Move to (0, 0)
	switch (get_client_border_type_()){
	case border_type::round_rect:
		set_handle_(CreateRoundRectRgn(region.left, region.top, region.right, region.bottom, border_curve_size.cx, border_curve_size.cy));
		break;
	case border_type::ellipse:
		set_handle_(CreateEllipticRgnIndirect(&region));
		break;
	case border_type::complex:
		set_handle_(create_complex_client_region_());
		break;
	default:
		set_handle_(CreateRectRgnIndirect(&region));
		break;
	}

	return (get_handle_() != nullptr);
}

bool winp::non_window::child::destroy_non_client_(){
	if (non_client_handle_ == nullptr)
		return true;

	if (DeleteObject(non_client_handle_) == FALSE)
		return false;

	non_client_handle_ = nullptr;
	return true;
}

bool winp::non_window::child::destroy_client_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return true;

	if (DeleteObject(handle) == FALSE)
		return false;

	set_handle_(nullptr);
	return true;
}

bool winp::non_window::child::should_create_non_client_region_() const{
	auto border_type = get_non_client_border_type_();
	if (border_type == border_type::complex)
		return true;

	auto &padding = get_padding_();
	auto no_padding = (padding.left == 0 && padding.top == 0 && padding.right == 0 && padding.bottom == 0);

	auto &non_client_border_curve_size = get_non_client_border_curve_size_(), &client_border_curve_size = get_client_border_curve_size_();
	auto is_same_border_curve_size = (border_type == get_non_client_border_type_() && non_client_border_curve_size.cx == client_border_curve_size.cx && non_client_border_curve_size.cy == client_border_curve_size.cy);

	return (!no_padding || !is_same_border_curve_size || !compare_colors(background_color_, non_client_background_color_));
}

bool winp::non_window::child::set_border_type_(border_type value){
	auto is_same_non_client = (value == get_non_client_border_type_()), is_same_client = (value == get_client_border_type_());
	if (is_same_non_client && is_same_client)
		return true;//No changes

	client_border_type_ = non_client_border_type_ = value;
	if (!is_same_non_client && !is_same_client)
		return border_details_changed_(changed_border_type::both);

	return border_details_changed_((is_same_non_client ? changed_border_type::client : changed_border_type::non_client));
}

bool winp::non_window::child::set_non_client_border_type_(border_type value){
	if (value == get_non_client_border_type_())
		return true;

	non_client_border_type_ = value;
	return border_details_changed_(changed_border_type::non_client);
}

winp::non_window::child::border_type winp::non_window::child::get_non_client_border_type_() const{
	return non_client_border_type_;
}

bool winp::non_window::child::set_client_border_type_(border_type value){
	if (value == get_client_border_type_())
		return true;

	client_border_type_ = value;
	return border_details_changed_(changed_border_type::client);
}

winp::non_window::child::border_type winp::non_window::child::get_client_border_type_() const{
	return client_border_type_;
}

bool winp::non_window::child::set_border_curve_size_(const m_size_type &value){
	auto is_same_non_client = (value.cx == non_client_border_curve_size_.cx && value.cy == non_client_border_curve_size_.cy);
	auto is_same_client = (value.cx == client_border_curve_size_.cx && value.cy == client_border_curve_size_.cy);
	if (is_same_non_client && is_same_client)
		return true;//No changes

	client_border_curve_size_ = non_client_border_curve_size_ = value;
	if (!is_same_non_client && !is_same_client)
		return border_details_changed_(changed_border_type::both);

	return border_details_changed_((is_same_non_client ? changed_border_type::client : changed_border_type::non_client));
}

bool winp::non_window::child::set_non_client_border_curve_size_(const m_size_type &value){
	if (value.cx == non_client_border_curve_size_.cx && value.cy == non_client_border_curve_size_.cy)
		return true;//No changes

	non_client_border_curve_size_ = value;
	return border_details_changed_(changed_border_type::non_client);
}

const winp::ui::surface::m_size_type &winp::non_window::child::get_non_client_border_curve_size_() const{
	return non_client_border_curve_size_;
}

bool winp::non_window::child::set_client_border_curve_size_(const m_size_type &value){
	if (value.cx == client_border_curve_size_.cx && value.cy == client_border_curve_size_.cy)
		return true;//No changes

	client_border_curve_size_ = value;
	return border_details_changed_(changed_border_type::client);
}

const winp::ui::surface::m_size_type &winp::non_window::child::get_client_border_curve_size_() const{
	return client_border_curve_size_;
}

bool winp::non_window::child::border_details_changed_(changed_border_type type){
	if (get_handle_() == nullptr)
		return true;//Not created

	if ((type == changed_border_type::non_client || type == changed_border_type::both) && (!destroy_non_client_() || !create_non_client_()))
		return false;

	if ((type == changed_border_type::client || type == changed_border_type::both) && (!destroy_client_() || !create_client_()))
		return false;

	redraw_(m_rect_type{});
	return true;
}

HRGN winp::non_window::child::create_complex_non_client_region_() const{
	return nullptr;
}

HRGN winp::non_window::child::create_complex_client_region_() const{
	return nullptr;
}
