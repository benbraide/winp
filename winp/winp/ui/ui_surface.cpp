#include "ui_surface.h"

winp::ui::surface::surface() = default;

winp::ui::surface::surface(thread::object &thread)
	: tree(thread){}

winp::ui::surface::~surface() = default;

bool winp::ui::surface::set_size(const m_size_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_size_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_size_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::surface::offset_size(const m_size_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = offset_size_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = offset_size_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::ui::surface::m_size_type winp::ui::surface::get_size(const std::function<void(const m_size_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_size_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_size_()); }, thread::queue::send_priority, id_);
		return m_size_type{};
	}

	return thread_.queue.execute([this]{ return get_size_(); }, thread::queue::send_priority, id_);
}

winp::ui::surface::m_size_type winp::ui::surface::get_client_position_offset(const std::function<void(const m_size_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_client_position_offset_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_client_position_offset_()); }, thread::queue::send_priority, id_);
		return m_size_type{};
	}

	return thread_.queue.execute([this]{ return get_client_position_offset_(); }, thread::queue::send_priority, id_);
}

bool winp::ui::surface::set_position(const m_point_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_position_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_position_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::surface::offset_position(const m_size_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = offset_position_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = offset_position_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::ui::surface::m_point_type winp::ui::surface::get_position(const std::function<void(const m_point_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_position_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_position_()); }, thread::queue::send_priority, id_);
		return m_point_type{};
	}

	return thread_.queue.execute([this]{ return get_position_(); }, thread::queue::send_priority, id_);
}

bool winp::ui::surface::set_absolute_position(const m_point_type &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_absolute_position_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_absolute_position_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::ui::surface::m_point_type winp::ui::surface::get_absolute_position(const std::function<void(const m_point_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_absolute_position_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_position_()); }, thread::queue::send_priority, id_);
		return m_point_type{};
	}

	return thread_.queue.execute([this]{ return get_absolute_position_(); }, thread::queue::send_priority, id_);
}

winp::ui::surface::m_rect_type winp::ui::surface::get_dimension(const std::function<void(const m_rect_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_dimension_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_dimension_()); }, thread::queue::send_priority, id_);
		return m_rect_type{};
	}

	return thread_.queue.execute([this]{ return get_dimension_(); }, thread::queue::send_priority, id_);
}

winp::ui::surface::m_rect_type winp::ui::surface::get_absolute_dimension(const std::function<void(const m_rect_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_absolute_dimension_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_dimension_()); }, thread::queue::send_priority, id_);
		return m_rect_type{};
	}

	return thread_.queue.execute([this]{ return get_absolute_dimension_(); }, thread::queue::send_priority, id_);
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_from_absolute_value(const m_point_type &value, const std::function<void(const m_point_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = convert_position_from_absolute_value_(value);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(convert_position_from_absolute_value_(value)); }, thread::queue::send_priority, id_);
		return m_point_type{};
	}

	return thread_.queue.execute([=]{ return convert_position_from_absolute_value_(value); }, thread::queue::send_priority, id_);
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_to_absolute_value(const m_point_type &value, const std::function<void(const m_point_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = convert_position_to_absolute_value_(value);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(convert_position_to_absolute_value_(value)); }, thread::queue::send_priority, id_);
		return m_point_type{};
	}

	return thread_.queue.execute([=]{ return convert_position_to_absolute_value_(value); }, thread::queue::send_priority, id_);
}

winp::ui::surface::m_rect_type winp::ui::surface::convert_dimension_from_absolute_value(const m_rect_type &value, const std::function<void(const m_rect_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = convert_dimension_from_absolute_value_(value);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(convert_dimension_from_absolute_value_(value)); }, thread::queue::send_priority, id_);
		return m_rect_type{};
	}

	return thread_.queue.execute([=]{ return convert_dimension_from_absolute_value_(value); }, thread::queue::send_priority, id_);
}

winp::ui::surface::m_rect_type winp::ui::surface::convert_dimension_to_absolute_value(const m_rect_type &value, const std::function<void(const m_rect_type &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = convert_dimension_to_absolute_value_(value);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(convert_dimension_to_absolute_value_(value)); }, thread::queue::send_priority, id_);
		return m_rect_type{};
	}

	return thread_.queue.execute([=]{ return convert_dimension_to_absolute_value_(value); }, thread::queue::send_priority, id_);
}

UINT winp::ui::surface::hit_test(const m_point_type &pt, bool is_absolute, const std::function<void(UINT)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = hit_test_(pt, is_absolute);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(hit_test_(pt, is_absolute)); }, thread::queue::send_priority, id_);
		return HTNOWHERE;
	}

	return thread_.queue.execute([=]{ return hit_test_(pt, is_absolute); }, thread::queue::send_priority, id_);
}

winp::utility::hit_target winp::ui::surface::hit_test(const m_rect_type &rect, bool is_absolute, const std::function<void(utility::hit_target)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = hit_test_(rect, is_absolute);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(hit_test_(rect, is_absolute)); }, thread::queue::send_priority, id_);
		return utility::hit_target::nil;
	}

	return thread_.queue.execute([=]{ return hit_test_(rect, is_absolute); }, thread::queue::send_priority, id_);
}

winp::utility::hit_target winp::ui::surface::hit_test(const m_point_type &pt, const m_point_type &pos, const m_size_type &size, const std::function<void(utility::hit_target)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = hit_test_(pt, pos, size);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(hit_test_(pt, pos, size)); }, thread::queue::send_priority, id_);
		return utility::hit_target::nil;
	}

	return thread_.queue.execute([=]{ return hit_test_(pt, pos, size); }, thread::queue::send_priority, id_);
}

void winp::ui::surface::add_to_toplevel_(bool update){
	if (get_parent_() == nullptr)
		thread_.surface_manager_.toplevel_map_[static_cast<HWND>(get_handle_())] = this;
	else if (update)//Remove from top level list
		thread_.surface_manager_.toplevel_map_.erase(static_cast<HWND>(get_handle_()));
}

winp::ui::surface *winp::ui::surface::get_surface_parent_() const{
	return dynamic_cast<surface *>(get_parent_());
}

winp::ui::surface *winp::ui::surface::get_root_surface_() const{
	auto parent = get_surface_parent_();
	return ((parent == nullptr) ? const_cast<surface *>(this) : parent->get_root_surface_());
}

void winp::ui::surface::set_message_entry_(LONG_PTR value){}

bool winp::ui::surface::set_size_(const m_size_type &value){
	size_ = value;
	return true;
}

bool winp::ui::surface::offset_size_(const m_size_type &value){
	auto size = get_size_();
	return set_size_(m_size_type{ (size.cx + value.cx), (size.cy + value.cy) });
}

winp::ui::surface::m_size_type winp::ui::surface::get_size_() const{
	return size_;
}

winp::ui::surface::m_size_type winp::ui::surface::get_client_position_offset_() const{
	return m_size_type{};
}

bool winp::ui::surface::set_position_(const m_point_type &value){
	position_ = value;
	return true;
}

bool winp::ui::surface::offset_position_(const m_size_type &value){
	auto position = get_position_();
	return set_position_(m_point_type{ (position.x + value.cx), (position.y + value.cy) });
}

winp::ui::surface::m_point_type winp::ui::surface::get_position_() const{
	return position_;
}

winp::ui::surface::m_point_type winp::ui::surface::get_cursor_position_() const{
	return m_point_type{};
}

bool winp::ui::surface::set_absolute_position_(const m_point_type &value){
	auto surface_parent = get_surface_parent_();
	return set_position_((surface_parent == nullptr) ? value : surface_parent->convert_position_from_absolute_value_(value));
}

winp::ui::surface::m_point_type winp::ui::surface::get_absolute_position_() const{
	auto surface_parent = get_surface_parent_();
	return ((surface_parent == nullptr) ? get_position_() : surface_parent->convert_position_to_absolute_value_(get_position_()));
}

winp::ui::surface::m_rect_type winp::ui::surface::get_dimension_() const{
	auto size = get_size_();
	auto position = get_position_();
	
	return m_rect_type{
		position.x,
		position.y,
		(position.x + size.cx),
		(position.y + size.cy)
	};
}

winp::ui::surface::m_rect_type winp::ui::surface::get_absolute_dimension_() const{
	auto size = get_size_();
	auto position = get_absolute_position_();

	return m_rect_type{
		position.x,
		position.y,
		(position.x + size.cx),
		(position.y + size.cy)
	};
}

winp::ui::surface::m_rect_type winp::ui::surface::get_client_dimension_() const{
	auto size = get_size_();
	return m_rect_type{ 0, 0, size.cx, size.cy };
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_from_absolute_value_(const m_point_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	return m_point_type{ (value.x - absolute_position.x - client_offset.cx), (value.y - absolute_position.y - client_offset.cy) };
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_to_absolute_value_(const m_point_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	return m_point_type{ (value.x + absolute_position.x + client_offset.cx), (value.y + absolute_position.y + client_offset.cy) };
}

winp::ui::surface::m_rect_type winp::ui::surface::convert_dimension_from_absolute_value_(const m_rect_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	auto h_offset = (absolute_position.x - client_offset.cx);
	auto v_offset = (absolute_position.y - client_offset.cy);

	return m_rect_type{ (value.left - h_offset), (value.top - v_offset), (value.right - h_offset), (value.bottom - v_offset) };
}

winp::ui::surface::m_rect_type winp::ui::surface::convert_dimension_to_absolute_value_(const m_rect_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	auto h_offset = (absolute_position.x + client_offset.cx);
	auto v_offset = (absolute_position.y + client_offset.cy);

	return m_rect_type{ (value.left + h_offset), (value.top + v_offset), (value.right + h_offset), (value.bottom + v_offset) };
}

UINT winp::ui::surface::hit_test_(const m_point_type &pt, bool is_absolute) const{
	auto pos = (is_absolute ? get_absolute_position_() : get_position_());
	auto client_offset = get_client_position_offset_();
	{//Update position
		pos.x += client_offset.cx;
		pos.y += client_offset.cy;
	}

	return ((hit_test_(pt, pos, get_size_()) == utility::hit_target::inside) ? HTCLIENT : HTNOWHERE);
}

winp::utility::hit_target winp::ui::surface::hit_test_(const m_rect_type &rect, bool is_absolute) const{
	auto pos = (is_absolute ? get_absolute_position_() : get_position_());
	auto client_offset = get_client_position_offset_();
	{//Update position
		pos.x += client_offset.cx;
		pos.y += client_offset.cy;
	}

	auto size = get_size_();
	if (hit_test_(m_point_type{ rect.left, rect.top }, pos, size) == utility::hit_target::inside && hit_test_(m_point_type{ rect.right, rect.bottom }, pos, size) == utility::hit_target::inside)
		return utility::hit_target::inside;

	if (pos.x < rect.right && pos.y < rect.bottom && (pos.x + size.cx) > rect.left && (pos.y + size.cy) > rect.top)
		return utility::hit_target::intersect;

	return utility::hit_target::nil;
}

winp::utility::hit_target winp::ui::surface::hit_test_(const m_point_type &pt, const m_point_type &pos, const m_size_type &size) const{
	if (pt.x < pos.x || pt.y < pos.y)
		return utility::hit_target::nil;

	if (pt.x < (pos.x + size.cx) || pt.y < (pos.y + size.cy))
		return utility::hit_target::inside;

	return utility::hit_target::nil;
}
