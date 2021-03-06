#include "../app/app_object.h"

winp::ui::window_surface::window_surface() = default;

winp::ui::window_surface::window_surface(thread::object &thread)
	: io_surface(thread){}

winp::ui::window_surface::~window_surface(){
	destruct();
}

bool winp::ui::window_surface::show(int how, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, show_(how));
	});
}

bool winp::ui::window_surface::hide(const std::function<void(thread::item &, bool)> &callback){
	return show(SW_HIDE, callback);
}

bool winp::ui::window_surface::maximize(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, maximize_());
	});
}

bool winp::ui::window_surface::restore_maximized(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, restore_maximized_());
	});
}

bool winp::ui::window_surface::toggle_maximized(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, toggle_maximized_());
	});
}

bool winp::ui::window_surface::is_maximized(const std::function<void(bool)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, is_maximized_());
	}, callback != nullptr);
}

bool winp::ui::window_surface::minimize(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, minimize_());
	});
}

bool winp::ui::window_surface::restore_minimized(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, restore_minimized_());
	});
}

bool winp::ui::window_surface::toggle_minimized(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, toggle_minimized_());
	});
}

bool winp::ui::window_surface::is_minimized(const std::function<void(bool)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, is_minimized_());
	}, callback != nullptr);
}

bool winp::ui::window_surface::set_styles(DWORD value, bool is_extended, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_styles_(value, is_extended));
	});
}

bool winp::ui::window_surface::add_styles(DWORD value, bool is_extended, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, add_styles_(value, is_extended));
	});
}

bool winp::ui::window_surface::remove_styles(DWORD value, bool is_extended, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, remove_styles_(value, is_extended));
	});
}

DWORD winp::ui::window_surface::get_styles(bool is_extended, const std::function<void(DWORD)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_styles_(is_extended));
	}, callback != nullptr);
}

bool winp::ui::window_surface::has_styles(DWORD value, bool is_extended, bool has_all, const std::function<void(bool)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, has_styles_(value, is_extended, has_all));
	}, callback != nullptr);
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

	auto styles = (styles_ | get_persistent_styles_() | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	auto extended_styles = (extended_styles_ | get_persistent_extended_styles_());
	auto offset_from_window_ancestor = compute_offset_from_ancestor_of_<window_surface>();

	thread_.surface_manager_.cache_.creating = this;
	auto result = CreateWindowExW(
		extended_styles,
		get_class_name_(),
		get_window_text_(),
		styles,
		offset_from_window_ancestor.x,
		offset_from_window_ancestor.y,
		size_.cx,
		size_.cy,
		static_cast<HWND>(parent_handle),
		nullptr,
		get_instance_(),
		static_cast<surface *>(this)
	);

	if (result != nullptr){
		post_create_();
		update_padding_();
	}

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

bool winp::ui::window_surface::is_created_() const{
	return (get_handle_() != nullptr);
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

winp::ui::surface::m_point_type winp::ui::window_surface::get_client_position_offset_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_client_position_offset_();

	POINT client_offset{ 0, 0 };
	ClientToScreen(static_cast<HWND>(handle), &client_offset);

	RECT window_rect{};
	GetWindowRect(static_cast<HWND>(handle), &window_rect);

	return m_point_type{ (client_offset.x - window_rect.left), (client_offset.y - window_rect.top) };
}

winp::ui::surface::m_point_type winp::ui::window_surface::compute_child_observable_offset_(const surface &child) const{
	return child.get_position_();
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

bool winp::ui::window_surface::set_dimension_(const m_point_type &offset, const m_size_type &size){
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::set_dimension_(offset, size);

	return (SetWindowPos(static_cast<HWND>(handle), nullptr, offset.x, offset.y, size.cx, size.cy, (SWP_NOZORDER | SWP_NOACTIVATE)) != FALSE);
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

bool winp::ui::window_surface::set_padding_(const m_rect_type &value){
	return false;
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

void winp::ui::window_surface::update_padding_(){
	auto dimension = get_absolute_dimension_(), client_dimension = convert_dimension_to_absolute_value_(get_client_dimension_());
	{//Update 
		padding_.left = (client_dimension.left - dimension.left);
		padding_.top = (client_dimension.top - dimension.top);
		padding_.right = (dimension.right - client_dimension.right);
		padding_.bottom = (dimension.bottom - client_dimension.bottom);
	}
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

bool winp::ui::window_surface::update_styles_(bool is_extended){
	auto computed_value = compute_styles_((is_extended ? extended_styles_ : styles_), is_extended);
	if (computed_value == (is_extended ? extended_styles_ : styles_))
		return true;//No changes

	(is_extended ? extended_styles_ : styles_) = computed_value;
	if (auto handle = get_handle_(); handle != nullptr){//Update window
		SetWindowLongPtrW(static_cast<HWND>(handle), (is_extended ? GWL_EXSTYLE : GWL_STYLE), computed_value);
		SetWindowPos(static_cast<HWND>(handle), nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	return true;
}

bool winp::ui::window_surface::set_styles_(DWORD value, bool is_extended){
	auto computed_value = compute_styles_(value, is_extended);
	if (computed_value == (is_extended ? extended_styles_ : styles_))
		return true;//No changes

	(is_extended ? extended_styles_ : styles_) = computed_value;
	if (auto handle = get_handle_(); handle != nullptr){//Update window
		SetWindowLongPtrW(static_cast<HWND>(handle), (is_extended ? GWL_EXSTYLE : GWL_STYLE), computed_value);
		SetWindowPos(static_cast<HWND>(handle), nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	return true;
}

bool winp::ui::window_surface::add_styles_(DWORD value, bool is_extended){
	return (is_extended ? set_styles_((extended_styles_ | value), true) : set_styles_((styles_ | value), false));
}

bool winp::ui::window_surface::remove_styles_(DWORD value, bool is_extended){
	return (is_extended ? set_styles_((extended_styles_ & ~value), true) : set_styles_((styles_ & ~value), false));
}

bool winp::ui::window_surface::has_styles_(DWORD value, bool is_extended, bool has_all) const{
	return (has_all ? ((get_styles_(is_extended) & value) == value) : ((get_styles_(is_extended) & value) != 0u));
}

DWORD winp::ui::window_surface::get_styles_(bool is_extended) const{
	return (is_extended ? (extended_styles_ | get_persistent_extended_styles_()) : (styles_ | get_persistent_styles_()));
}

DWORD winp::ui::window_surface::get_persistent_styles_() const{
	return ((get_parent_() == nullptr) ? WS_POPUP : WS_CHILD);
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

DWORD winp::ui::window_surface::compute_styles_(DWORD value, bool is_extended) const{
	return (is_extended ? ((value & ~get_filtered_extended_styles_()) | get_persistent_extended_styles_()) : ((value & ~get_filtered_styles_()) | get_persistent_styles_()));
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
