#include "../app/app_object.h"

winp::ui::window_surface::window_surface(thread::object &thread)
	: io_surface(thread){
	create_event.thread_ = thread_;
	destroy_event.thread_ = thread_;
}

winp::ui::window_surface::~window_surface(){
	thread_->queue.add([=]{
		destroy_();
	}, thread::queue::send_priority, id_).get();
}

void winp::ui::window_surface::maximize(const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = maximize_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::window_surface::restore_maximized(const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = restore_maximized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::window_surface::toggle_maximized(const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = toggle_maximized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

bool winp::ui::window_surface::is_maximized(const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(is_maximized_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_->queue.add([this]{ return is_maximized_(); }, thread::queue::send_priority, id_).get();
}

void winp::ui::window_surface::minimize(const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = minimize_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::window_surface::restore_minimized(const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = restore_minimized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::window_surface::toggle_minimized(const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = toggle_minimized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

bool winp::ui::window_surface::is_minimized(const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(is_minimized_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_->queue.add([this]{ return is_minimized_(); }, thread::queue::send_priority, id_).get();
}

void winp::ui::window_surface::set_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = set_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::window_surface::add_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = add_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::window_surface::remove_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback){
	thread_->queue.post([=]{
		auto result = remove_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

DWORD winp::ui::window_surface::get_styles(bool is_extended, const std::function<void(DWORD)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(get_styles_(is_extended)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_->queue.add([=]{ return get_styles_(is_extended); }, thread::queue::send_priority, id_).get();
}

bool winp::ui::window_surface::has_styles(DWORD value, bool is_extended, bool has_all, const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_->queue.post([=]{ callback(has_styles_(value, is_extended, has_all)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_->queue.add([=]{ return has_styles_(value, is_extended, has_all); }, thread::queue::send_priority, id_).get();
}

bool winp::ui::window_surface::create_(){
	if (get_handle_() != nullptr)
		return true;//Already created

	auto parent_handle = get_first_window_ancestor_handle_();
	if (parent_handle == nullptr && get_parent_() != nullptr)
		return false;

	auto styles = (styles_ | get_persistent_styles_());
	auto extended_styles = (extended_styles_ | get_persistent_extended_styles_());

	thread_->windows_manager_.cache_.handle = nullptr;
	thread_->windows_manager_.cache_.object = this;

	auto offset_from_window_ancestor = get_offset_from_ancestor_of_<window_surface>(m_point_type{});
	auto result = CreateWindowExW(
		extended_styles,
		get_class_name_(),
		get_window_text_(),
		styles,
		(position_.x + offset_from_window_ancestor.x),
		(position_.y + offset_from_window_ancestor.y),
		size_.cx,
		size_.cy,
		parent_handle,
		nullptr,
		get_instance_(),
		this
	);

	return (result != nullptr);
}

bool winp::ui::window_surface::destroy_(){
	auto handle = get_handle_();
	return ((handle == nullptr) ? true : (DestroyWindow(handle) != FALSE));
}

WNDPROC winp::ui::window_surface::get_default_message_entry_() const{
	return DefWindowProcW;
}

bool winp::ui::window_surface::set_size_(const m_size_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::set_size_(value);
	
	return (SetWindowPos(handle, nullptr, 0, 0, value.cx, value.cy, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE)) != FALSE);
}

winp::ui::surface::m_size_type winp::ui::window_surface::get_size_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_size_();

	RECT dimension{};
	GetWindowRect(handle, &dimension);

	return m_size_type{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
}

winp::ui::surface::m_size_type winp::ui::window_surface::get_client_position_offset_() const{
	if (handle_ == nullptr)
		return io_surface::get_client_position_offset_();

	POINT client_offset{ 0, 0 };
	ClientToScreen(handle_, &client_offset);

	RECT window_rect{};
	GetWindowRect(handle_, &window_rect);

	return m_size_type{ (client_offset.x - window_rect.left), (client_offset.y - window_rect.top) };
}

bool winp::ui::window_surface::set_position_(const m_point_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::set_position_(value);
	
	return (SetWindowPos(handle, nullptr, value.x, value.y, 0, 0, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE)) != FALSE);
}

winp::ui::surface::m_point_type winp::ui::window_surface::get_position_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_position_();

	RECT dimension{};
	GetWindowRect(handle, &dimension);
	m_point_type value{ dimension.left, dimension.top };

	auto parent = get_surface_parent_();
	return ((parent == nullptr) ? value : parent->convert_position_from_absolute_value_(value));
}

winp::ui::surface::m_point_type winp::ui::window_surface::get_absolute_position_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_absolute_position_();

	RECT dimension{};
	GetWindowRect(handle, &dimension);

	return m_point_type{ dimension.left, dimension.top };
}

winp::ui::surface::m_rect_type winp::ui::window_surface::get_dimension_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_dimension_();

	RECT dimension{};
	GetWindowRect(handle, &dimension);

	auto parent = get_surface_parent_();
	return ((parent == nullptr) ? dimension : parent->convert_dimension_from_absolute_value_(dimension));
}

winp::ui::surface::m_rect_type winp::ui::window_surface::get_absolute_dimension_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_absolute_dimension_();

	RECT dimension{};
	GetWindowRect(handle, &dimension);

	return dimension;
}

winp::ui::surface::m_rect_type winp::ui::window_surface::get_client_dimension_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_client_dimension_();

	RECT dimension{};
	GetClientRect(handle, &dimension);

	return dimension;
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_from_absolute_value_(const m_point_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_position_from_absolute_value_(value);

	POINT p{ value.x, value.y };
	ScreenToClient(handle, &p);

	return p;
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_to_absolute_value_(const m_point_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_position_to_absolute_value_(value);

	POINT p{ value.x, value.y };
	ClientToScreen(handle, &p);

	return p;
}

winp::ui::surface::m_rect_type winp::ui::window_surface::convert_dimension_from_absolute_value_(const m_rect_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_dimension_from_absolute_value_(value);

	RECT r{ value.left, value.top, value.right, value.bottom };
	MapWindowPoints(HWND_DESKTOP, handle, reinterpret_cast<POINT *>(&r), (sizeof(RECT) / sizeof(POINT)));

	return r;
}

winp::ui::surface::m_rect_type winp::ui::window_surface::convert_dimension_to_absolute_value_(const m_rect_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_dimension_to_absolute_value_(value);

	RECT r{ value.left, value.top, value.right, value.bottom };
	MapWindowPoints(handle, HWND_DESKTOP, reinterpret_cast<POINT *>(&r), (sizeof(RECT) / sizeof(POINT)));

	return r;
}

void winp::ui::window_surface::redraw_(){
	auto handle = get_handle_();
	if (handle != nullptr)
		InvalidateRect(handle, nullptr, TRUE);
}

bool winp::ui::window_surface::set_visibility_(bool is_visible){
	auto handle = get_handle_();
	if (handle == nullptr){
		if (is_visible)
			return add_styles_(WS_VISIBLE, false);
		return remove_styles_(WS_VISIBLE, false);
	}
	
	if ((IsWindowVisible(handle) == FALSE) == is_visible)
		return (ShowWindow(handle, (is_visible ? SW_SHOW : SW_HIDE)) != FALSE);

	return true;
}

bool winp::ui::window_surface::is_visible_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_VISIBLE, false, true);
	return (IsWindowVisible(handle) != FALSE);
}

bool winp::ui::window_surface::set_transparency_(bool is_transparent){
	return (is_transparent ? add_styles_(WS_EX_TRANSPARENT, true) : remove_styles_(WS_EX_TRANSPARENT, true));
}

bool winp::ui::window_surface::is_transparent_() const{
	return has_styles_(WS_EX_TRANSPARENT, true, true);
}

winp::utility::hit_target winp::ui::window_surface::hit_test_(const m_point_type &pt, bool is_absolute) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::hit_test_(pt, is_absolute);

	auto absolute_pt = (is_absolute ? pt : convert_position_to_absolute_value_(pt));
	switch (SendMessageW(handle, WM_NCHITTEST, 0, MAKELONG(absolute_pt.x, absolute_pt.y))){
	case HTNOWHERE:
	case HTERROR://Outside window
		return utility::hit_target::nil;
	default:
		break;
	}

	return utility::hit_target::inside;
}

winp::utility::hit_target winp::ui::window_surface::hit_test_(const m_rect_type &rect, bool is_absolute) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::hit_test_(rect, is_absolute);

	bool first_pt_inside, second_pt_inside;
	auto absolute_rect = (is_absolute ? rect : convert_dimension_to_absolute_value_(rect));

	switch (SendMessageW(handle, WM_NCHITTEST, 0, MAKELONG(absolute_rect.left, absolute_rect.top))){
	case HTNOWHERE:
	case HTERROR://Outside window
		first_pt_inside = false;
		break;
	default:
		first_pt_inside = true;
		break;
	}

	switch (SendMessageW(handle, WM_NCHITTEST, 0, MAKELONG(absolute_rect.right, absolute_rect.bottom))){
	case HTNOWHERE:
	case HTERROR://Outside window
		second_pt_inside = false;
		break;
	default:
		second_pt_inside = true;
		break;
	}

	if (first_pt_inside && second_pt_inside)
		return utility::hit_target::inside;

	return ((first_pt_inside || second_pt_inside) ? utility::hit_target::intersect : utility::hit_target::nil);
}

winp::ui::window_surface *winp::ui::window_surface::get_window_surface_parent_() const{
	return dynamic_cast<window_surface *>(get_parent_());
}

bool winp::ui::window_surface::maximize_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return add_styles_(WS_MAXIMIZE, false);

	if (IsZoomed(handle) == FALSE)
		return (ShowWindow(handle, SW_MAXIMIZE) != FALSE);

	return true;
}

bool winp::ui::window_surface::restore_maximized_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return remove_styles_(WS_MAXIMIZE, false);

	if (IsZoomed(handle) != FALSE)
		return (ShowWindow(handle, SW_RESTORE) != FALSE);

	return true;
}

bool winp::ui::window_surface::toggle_maximized_(){
	return (is_maximized_() ? restore_maximized_() : maximize_());
}

bool winp::ui::window_surface::is_maximized_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_MAXIMIZE, false, true);

	return (IsZoomed(handle) != FALSE);
}

bool winp::ui::window_surface::minimize_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return add_styles_(WS_MINIMIZE, false);

	if (IsIconic(handle) == FALSE)
		return (ShowWindow(handle, SW_MINIMIZE) != FALSE);

	return true;
}

bool winp::ui::window_surface::restore_minimized_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return remove_styles_(WS_MINIMIZE, false);

	if (IsIconic(handle) != FALSE)
		return (ShowWindow(handle, SW_RESTORE) != FALSE);

	return true;
}

bool winp::ui::window_surface::toggle_minimized_(){
	return (is_minimized_() ? restore_minimized_() : minimize_());
}

bool winp::ui::window_surface::is_minimized_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_MINIMIZE, false, true);

	return (IsIconic(handle) != FALSE);
}

bool winp::ui::window_surface::set_styles_(DWORD value, bool is_extended){
	DWORD diff;
	HWND handle;

	if (is_extended){
		diff = extended_styles_;
		extended_styles_ = (value & ~(get_persistent_extended_styles_() | get_filtered_extended_styles_()));
		value = (extended_styles_ | get_persistent_extended_styles_());
		diff ^= extended_styles_;
	}
	else{//Basic styles
		diff = styles_;
		styles_ = (value & ~(get_persistent_styles_() | get_filtered_styles_()));
		value = (styles_ | get_persistent_styles_());
		diff ^= styles_;
	}

	if (diff != 0u && (handle = get_handle_()) != nullptr){//Update window
		SetWindowLongPtrW(handle, (is_extended ? GWL_EXSTYLE : GWL_STYLE), value);
		SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	return true;
}

bool winp::ui::window_surface::add_styles_(DWORD value, bool is_extended){
	DWORD *target;
	if (is_extended){
		target = &extended_styles_;
		value &= ~(get_persistent_extended_styles_() | get_filtered_extended_styles_());
	}
	else{
		target = &styles_;
		value &= ~(get_persistent_styles_() | get_filtered_styles_());
	}

	if (value != 0u && (*target & value) != value)
		return set_styles_((*target | value), is_extended);

	return true;
}

bool winp::ui::window_surface::remove_styles_(DWORD value, bool is_extended){
	DWORD *target;
	if (is_extended){
		target = &extended_styles_;
		value &= ~(get_persistent_extended_styles_() | get_filtered_extended_styles_());
	}
	else{
		target = &styles_;
		value &= ~(get_persistent_styles_() | get_filtered_styles_());
	}

	if (value != 0u && (*target & value) != 0u)
		return set_styles_((*target & ~value), is_extended);

	return true;
}

bool winp::ui::window_surface::has_styles_(DWORD value, bool is_extended, bool has_all) const{
	return (has_all ? ((get_styles_(is_extended) & value) == value) : ((get_styles_(is_extended) & value) != 0u));
}

DWORD winp::ui::window_surface::get_styles_(bool is_extended) const{
	return (is_extended ? (extended_styles_ | get_persistent_extended_styles_()) : (styles_ | get_persistent_styles_()));
}

DWORD winp::ui::window_surface::get_persistent_styles_() const{
	return ((get_parent_() == nullptr) ? 0u : WS_CHILD);
}

DWORD winp::ui::window_surface::get_persistent_extended_styles_() const{
	return 0u;
}

DWORD winp::ui::window_surface::get_filtered_styles_() const{
	return 0u;
}

DWORD winp::ui::window_surface::get_filtered_extended_styles_() const{
	return 0u;
}

HINSTANCE winp::ui::window_surface::get_instance_() const{
	return GetModuleHandleW(nullptr);
}

const wchar_t *winp::ui::window_surface::get_class_name_() const{
	return WINP_CLASS_WUUID;
}

const wchar_t *winp::ui::window_surface::get_window_text_() const{
	return L"";
}

HWND winp::ui::window_surface::get_first_window_ancestor_handle_() const{
	auto window_ancestor = get_first_ancestor_of_<window_surface>();
	return ((window_ancestor == nullptr) ? nullptr : window_ancestor->get_handle_());
}
