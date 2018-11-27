#include "../app/app_object.h"

winp::ui::window_surface::window_surface() = default;

winp::ui::window_surface::window_surface(thread::object &thread)
	: io_surface(thread){}

winp::ui::window_surface::~window_surface(){
	destruct();
}

bool winp::ui::window_surface::show(int how, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = show_(how);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = show_(how);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::hide(const std::function<void(object &, bool)> &callback){
	return show(SW_HIDE, callback);
}

bool winp::ui::window_surface::maximize(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = maximize_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = maximize_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::restore_maximized(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = restore_maximized_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = restore_maximized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::toggle_maximized(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = toggle_maximized_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = toggle_maximized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::is_maximized(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_maximized_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_maximized_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_maximized_(); }, thread::queue::send_priority, id_);
}

bool winp::ui::window_surface::minimize(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = minimize_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = minimize_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::restore_minimized(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = restore_minimized_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = restore_minimized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::toggle_minimized(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = toggle_minimized_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = toggle_minimized_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::is_minimized(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_minimized_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_minimized_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_minimized_(); }, thread::queue::send_priority, id_);
}

bool winp::ui::window_surface::set_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::add_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = add_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = add_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::window_surface::remove_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = remove_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = remove_styles_(value, is_extended);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

DWORD winp::ui::window_surface::get_styles(bool is_extended, const std::function<void(DWORD)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_styles_(is_extended);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_styles_(is_extended)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([=]{ return get_styles_(is_extended); }, thread::queue::send_priority, id_);
}

bool winp::ui::window_surface::has_styles(DWORD value, bool is_extended, bool has_all, const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = has_styles_(value, is_extended, has_all);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(has_styles_(value, is_extended, has_all)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([=]{ return has_styles_(value, is_extended, has_all); }, thread::queue::send_priority, id_);
}

void winp::ui::window_surface::destruct_(){
	destroy_();
	io_surface::destruct_();
}

bool winp::ui::window_surface::create_(){
	if (get_handle_() != nullptr)
		return true;//Already created

	if (!pre_create_())
		return false;

	auto window_parent = get_first_ancestor_of_<window_surface>();
	auto parent_handle = ((window_parent == nullptr) ? nullptr : window_parent->get_handle_());

	if (parent_handle == nullptr && window_parent != nullptr)
		return false;//Parent not created

	auto styles = (styles_ | get_persistent_styles_());
	auto extended_styles = (extended_styles_ | get_persistent_extended_styles_());

	thread_.surface_manager_.cache_.creating = this;

	auto offset_from_window_ancestor = ((window_parent == nullptr) ? m_point_type{} : get_offset_from_ancestor_of_<window_surface>(m_point_type{}));
	auto result = CreateWindowExW(
		extended_styles,
		get_class_name_(),
		get_window_text_(),
		styles,
		(position_.x + offset_from_window_ancestor.x),
		(position_.y + offset_from_window_ancestor.y),
		size_.cx,
		size_.cy,
		static_cast<HWND>(parent_handle),
		nullptr,
		get_instance_(),
		static_cast<surface *>(this)
	);

	if (result != nullptr)
		post_create_();

	return (get_handle_() != nullptr);
}

bool winp::ui::window_surface::destroy_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return true;

	for (auto child : children_){//Check for menu
		if (dynamic_cast<menu::object *>(child) != nullptr){
			erase_child_(*child);
			break;
		}
	}

	return (DestroyWindow(static_cast<HWND>(handle)) != FALSE);
}

void winp::ui::window_surface::parent_changed_(tree *previous_parent, std::size_t previous_index){
	io_surface::parent_changed_(previous_parent, previous_index);
	if (get_handle_() == nullptr)
		return;//Not created

	auto parent = get_parent_();
	if (parent == nullptr){
		SetParent(static_cast<HWND>(get_handle_()), nullptr);
		set_styles_((get_styles_(false) & ~WS_CHILD), false);
	}
	else{
		set_styles_(((get_styles_(false) | WS_CHILD) & ~WS_POPUP), false);
		SetParent(static_cast<HWND>(get_handle_()), static_cast<HWND>(parent->get_handle_()));
	}
}

const wchar_t *winp::ui::window_surface::get_theme_name_() const{
	return L"WINDOW";
}

WNDPROC winp::ui::window_surface::get_default_message_entry_() const{
	return app::object::get_default_message_entry(get_class_name_());
}

void winp::ui::window_surface::set_message_entry_(LONG_PTR value){
	auto def_msg_entry = get_default_message_entry_();
	if (def_msg_entry != DefWindowProcW && reinterpret_cast<LONG_PTR>(def_msg_entry) != value)
		SetWindowLongPtrW(static_cast<HWND>(get_handle_()), GWLP_WNDPROC, value);
}

bool winp::ui::window_surface::set_size_(const m_size_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::set_size_(value);
	
	return (SetWindowPos(static_cast<HWND>(handle), nullptr, 0, 0, value.cx, value.cy, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE)) != FALSE);
}

winp::ui::surface::m_size_type winp::ui::window_surface::get_size_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_size_();

	RECT dimension{};
	GetWindowRect(static_cast<HWND>(handle), &dimension);

	return m_size_type{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
}

winp::ui::surface::m_size_type winp::ui::window_surface::get_client_position_offset_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_client_position_offset_();

	POINT client_offset{ 0, 0 };
	ClientToScreen(static_cast<HWND>(handle), &client_offset);

	RECT window_rect{};
	GetWindowRect(static_cast<HWND>(handle), &window_rect);

	return m_size_type{ (client_offset.x - window_rect.left), (client_offset.y - window_rect.top) };
}

bool winp::ui::window_surface::set_position_(const m_point_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::set_position_(value);
	
	return (SetWindowPos(static_cast<HWND>(handle), nullptr, value.x, value.y, 0, 0, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE)) != FALSE);
}

winp::ui::surface::m_point_type winp::ui::window_surface::get_position_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_position_();

	RECT dimension{};
	GetWindowRect(static_cast<HWND>(handle), &dimension);
	m_point_type value{ dimension.left, dimension.top };

	auto parent = get_surface_parent_();
	return ((parent == nullptr) ? value : parent->convert_position_from_absolute_value_(value));
}

winp::ui::surface::m_point_type winp::ui::window_surface::get_absolute_position_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_absolute_position_();

	RECT dimension{};
	GetWindowRect(static_cast<HWND>(handle), &dimension);

	return m_point_type{ dimension.left, dimension.top };
}

winp::ui::surface::m_rect_type winp::ui::window_surface::get_dimension_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_dimension_();

	RECT dimension{};
	GetWindowRect(static_cast<HWND>(handle), &dimension);

	auto parent = get_surface_parent_();
	return ((parent == nullptr) ? dimension : parent->convert_dimension_from_absolute_value_(dimension));
}

winp::ui::surface::m_rect_type winp::ui::window_surface::get_absolute_dimension_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_absolute_dimension_();

	RECT dimension{};
	GetWindowRect(static_cast<HWND>(handle), &dimension);

	return dimension;
}

winp::ui::surface::m_rect_type winp::ui::window_surface::get_client_dimension_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_client_dimension_();

	RECT dimension{};
	GetClientRect(static_cast<HWND>(handle), &dimension);

	return dimension;
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_from_absolute_value_(const m_point_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_position_from_absolute_value_(value);

	POINT p{ value.x, value.y };
	ScreenToClient(static_cast<HWND>(handle), &p);

	return p;
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_to_absolute_value_(const m_point_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_position_to_absolute_value_(value);

	POINT p{ value.x, value.y };
	ClientToScreen(static_cast<HWND>(handle), &p);

	return p;
}

winp::ui::surface::m_rect_type winp::ui::window_surface::convert_dimension_from_absolute_value_(const m_rect_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_dimension_from_absolute_value_(value);

	RECT r{ value.left, value.top, value.right, value.bottom };
	MapWindowPoints(HWND_DESKTOP, static_cast<HWND>(handle), reinterpret_cast<POINT *>(&r), (sizeof(RECT) / sizeof(POINT)));

	return r;
}

winp::ui::surface::m_rect_type winp::ui::window_surface::convert_dimension_to_absolute_value_(const m_rect_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_dimension_to_absolute_value_(value);

	RECT r{ value.left, value.top, value.right, value.bottom };
	MapWindowPoints(static_cast<HWND>(handle), HWND_DESKTOP, reinterpret_cast<POINT *>(&r), (sizeof(RECT) / sizeof(POINT)));

	return r;
}

void winp::ui::window_surface::redraw_(const m_rect_type &region){
	auto handle = get_handle_();
	if (handle == nullptr)
		return;

	if (region.left < region.right || region.top < region.bottom)
		InvalidateRect(static_cast<HWND>(handle), &region, TRUE);
	else
		InvalidateRect(static_cast<HWND>(handle), nullptr, TRUE);
}

bool winp::ui::window_surface::set_visibility_(bool is_visible){
	auto handle = get_handle_();
	if (handle == nullptr){
		if (is_visible)
			return add_styles_(WS_VISIBLE, false);
		return remove_styles_(WS_VISIBLE, false);
	}
	
	if ((IsWindowVisible(static_cast<HWND>(handle)) == FALSE) == is_visible)
		return (ShowWindow(static_cast<HWND>(handle), (is_visible ? SW_SHOW : SW_HIDE)) != FALSE);

	return true;
}

bool winp::ui::window_surface::is_visible_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_VISIBLE, false, true);
	return (IsWindowVisible(static_cast<HWND>(handle)) != FALSE);
}

UINT winp::ui::window_surface::hit_test_(const m_point_type &pt, bool is_absolute) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::hit_test_(pt, is_absolute);

	auto absolute_pt = (is_absolute ? pt : convert_position_to_absolute_value_(pt));
	return static_cast<UINT>(SendMessageW(static_cast<HWND>(handle), WM_NCHITTEST, 0, MAKELONG(absolute_pt.x, absolute_pt.y)));
}

winp::utility::hit_target winp::ui::window_surface::hit_test_(const m_rect_type &rect, bool is_absolute) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::hit_test_(rect, is_absolute);

	bool first_pt_inside, second_pt_inside;
	auto absolute_rect = (is_absolute ? rect : convert_dimension_to_absolute_value_(rect));

	switch (SendMessageW(static_cast<HWND>(handle), WM_NCHITTEST, 0, MAKELONG(absolute_rect.left, absolute_rect.top))){
	case HTNOWHERE:
	case HTERROR://Outside window
		first_pt_inside = false;
		break;
	default:
		first_pt_inside = true;
		break;
	}

	switch (SendMessageW(static_cast<HWND>(handle), WM_NCHITTEST, 0, MAKELONG(absolute_rect.right, absolute_rect.bottom))){
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

bool winp::ui::window_surface::is_dialog_message_(MSG &msg) const{
	auto handle = get_handle_();
	return (handle != nullptr && IsDialogMessageW(static_cast<HWND>(handle), &msg) != FALSE);
}

bool winp::ui::window_surface::pre_create_(){
	return true;
}

void winp::ui::window_surface::post_create_(){}

winp::ui::window_surface *winp::ui::window_surface::get_window_surface_parent_() const{
	return dynamic_cast<window_surface *>(get_parent_());
}

bool winp::ui::window_surface::show_(int how){
	auto handle = get_handle_();
	if (handle != nullptr)
		return (ShowWindow(static_cast<HWND>(handle), how) != FALSE);

	switch (how){
	case SW_HIDE:
		remove_styles_(WS_VISIBLE, false);
		break;
	case SW_MAXIMIZE:
		add_styles_(WS_MAXIMIZE, false);
		break;
	case SW_MINIMIZE:
		add_styles_(WS_MINIMIZE, false);
		break;
	default:
		break;
	}

	return add_styles_(WS_VISIBLE, false);
}

bool winp::ui::window_surface::maximize_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return add_styles_(WS_MAXIMIZE, false);

	if (IsZoomed(static_cast<HWND>(handle)) == FALSE)
		return (ShowWindow(static_cast<HWND>(handle), SW_MAXIMIZE) != FALSE);

	return true;
}

bool winp::ui::window_surface::restore_maximized_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return remove_styles_(WS_MAXIMIZE, false);

	if (IsZoomed(static_cast<HWND>(handle)) != FALSE)
		return (ShowWindow(static_cast<HWND>(handle), SW_RESTORE) != FALSE);

	return true;
}

bool winp::ui::window_surface::toggle_maximized_(){
	return (is_maximized_() ? restore_maximized_() : maximize_());
}

bool winp::ui::window_surface::is_maximized_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_MAXIMIZE, false, true);

	return (IsZoomed(static_cast<HWND>(handle)) != FALSE);
}

bool winp::ui::window_surface::minimize_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return add_styles_(WS_MINIMIZE, false);

	if (IsIconic(static_cast<HWND>(handle)) == FALSE)
		return (ShowWindow(static_cast<HWND>(handle), SW_MINIMIZE) != FALSE);

	return true;
}

bool winp::ui::window_surface::restore_minimized_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return remove_styles_(WS_MINIMIZE, false);

	if (IsIconic(static_cast<HWND>(handle)) != FALSE)
		return (ShowWindow(static_cast<HWND>(handle), SW_RESTORE) != FALSE);

	return true;
}

bool winp::ui::window_surface::toggle_minimized_(){
	return (is_minimized_() ? restore_minimized_() : minimize_());
}

bool winp::ui::window_surface::is_minimized_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_MINIMIZE, false, true);

	return (IsIconic(static_cast<HWND>(handle)) != FALSE);
}

bool winp::ui::window_surface::set_styles_(DWORD value, bool is_extended){
	DWORD diff;
	HANDLE handle;

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
		SetWindowLongPtrW(static_cast<HWND>(handle), (is_extended ? GWL_EXSTYLE : GWL_STYLE), value);
		SetWindowPos(static_cast<HWND>(handle), nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
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
	return ((get_parent_() == nullptr) ? WS_CHILD : WS_POPUP);
}

DWORD winp::ui::window_surface::get_filtered_extended_styles_() const{
	return 0u;
}

HINSTANCE winp::ui::window_surface::get_instance_() const{
	return GetModuleHandleW(nullptr);
}

const wchar_t *winp::ui::window_surface::get_class_name_() const{
	return (class_name_.empty() ? WINP_CLASS_WUUID : class_name_.data());
}

const wchar_t *winp::ui::window_surface::get_window_text_() const{
	return L"";
}

HWND winp::ui::window_surface::get_first_window_ancestor_handle_() const{
	auto window_ancestor = get_first_ancestor_of_<window_surface>();
	return ((window_ancestor == nullptr) ? nullptr : static_cast<HWND>(window_ancestor->get_handle_()));
}
