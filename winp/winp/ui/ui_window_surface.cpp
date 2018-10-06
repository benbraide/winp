#include "../app/app_object.h"

winp::ui::window_surface::window_surface(thread::object &thread)
	: io_surface(thread){
	init_();
}

winp::ui::window_surface::window_surface(tree &parent)
	: io_surface(thread){
	init_();
}

winp::ui::window_surface::~window_surface(){
	if (!app::object::is_shut_down){
		owner_->queue->add([=]{
			destroy_();
		}, thread::queue::send_priority).get();
	}
}

void winp::ui::window_surface::init_(){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
		if (&prop == &styles){
			auto tval = *static_cast<const DWORD *>(value);
			owner_->queue->post([=]{
				set_styles_(tval, false);
			}, thread::queue::send_priority);
		}
		else if (&prop == &extended_styles){
			auto tval = *static_cast<const DWORD *>(value);
			owner_->queue->post([=]{
				set_styles_(tval, true);
			}, thread::queue::send_priority);
		}
		else if (&prop == &created){
			auto tval = *static_cast<const bool *>(value);
			owner_->queue->post([=]{
				if (tval)
					create_();
				else
					destroy_();
			}, thread::queue::send_priority);
		}
		else if (&prop == &maximized){
			auto tval = *static_cast<const bool *>(value);
			owner_->queue->post([=]{
				set_maximized_state_(tval);
			}, thread::queue::send_priority);
		}
		else if (&prop == &minimized){
			auto tval = *static_cast<const bool *>(value);
			owner_->queue->post([=]{
				set_minimized_state_(tval);
			}, thread::queue::send_priority);
		}
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &styles)
			*static_cast<DWORD *>(buf) = owner_->queue->add([this]{ return get_styles_(false); }, thread::queue::send_priority).get();
		else if (&prop == &extended_styles)
			*static_cast<DWORD *>(buf) = owner_->queue->add([this]{ return get_styles_(true); }, thread::queue::send_priority).get();
		else if (&prop == &created)
			*static_cast<bool *>(buf) = owner_->queue->add([this]{ return (get_handle_() != nullptr); }, thread::queue::send_priority).get();
		else if (&prop == &maximized)
			*static_cast<bool *>(buf) = owner_->queue->add([this]{ return get_maximized_state_(); }, thread::queue::send_priority).get();
		else if (&prop == &minimized)
			*static_cast<bool *>(buf) = owner_->queue->add([this]{ return get_minimized_state_(); }, thread::queue::send_priority).get();
	};

	styles.init_(nullptr, setter, getter);
	extended_styles.init_(nullptr, setter, getter);

	created.init_(nullptr, setter, getter);
	maximized.init_(nullptr, setter, getter);
	minimized.init_(nullptr, setter, getter);

	create_event.thread_ = owner_;
	destroy_event.thread_ = owner_;

	handle_ = nullptr;
}

void winp::ui::window_surface::destroy_(){
	auto handle = get_handle_();
	if (handle != nullptr)
		DestroyWindow(handle);
}

void winp::ui::window_surface::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(window_surface *))
		*static_cast<window_surface **>(buf) = this;
	else
		io_surface::do_request_(buf, id);
}

void winp::ui::window_surface::do_apply_(const void *value, const std::type_info &id){

}

WNDPROC winp::ui::window_surface::get_default_message_entry_() const{
	return DefWindowProcW;
}

void winp::ui::window_surface::set_size_(const m_size_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		io_surface::set_size_(value);
	else
		SetWindowPos(handle, nullptr, 0, 0, value.width, value.height, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE));
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

void winp::ui::window_surface::set_position_(const m_point_type &value){
	auto handle = get_handle_();
	if (handle == nullptr)
		io_surface::set_position_(value);
	else
		SetWindowPos(handle, nullptr, value.x, value.y, 0, 0, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE));
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

	m_rect_type value{ dimension.left, dimension.top, dimension.right, dimension.bottom };
	auto parent = get_surface_parent_();

	return ((parent == nullptr) ? value : parent->convert_dimension_from_absolute_value_(value));
}

winp::ui::surface::m_rect_type winp::ui::window_surface::get_absolute_dimension_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::get_absolute_dimension_();

	RECT dimension{};
	GetWindowRect(handle, &dimension);

	return m_rect_type{ dimension.left, dimension.top, dimension.right, dimension.bottom };
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_from_absolute_value_(const m_point_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_position_from_absolute_value_(value);

	POINT p{ value.x, value.y };
	ScreenToClient(handle, &p);

	return m_point_type{ p.x, p.y };
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_to_absolute_value_(const m_point_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_position_to_absolute_value_(value);

	POINT p{ value.x, value.y };
	ClientToScreen(handle, &p);

	return m_point_type{ p.x, p.y };
}

winp::ui::surface::m_rect_type winp::ui::window_surface::convert_dimension_from_absolute_value_(const m_rect_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_dimension_from_absolute_value_(value);

	RECT r{ value.left, value.top, value.right, value.bottom };
	MapWindowPoints(HWND_DESKTOP, handle, reinterpret_cast<POINT *>(&r), (sizeof(RECT) / sizeof(POINT)));

	return m_rect_type{ r.left, r.top, r.right, r.bottom };
}

winp::ui::surface::m_rect_type winp::ui::window_surface::convert_dimension_to_absolute_value_(const m_rect_type &value) const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return io_surface::convert_dimension_to_absolute_value_(value);

	RECT r{ value.left, value.top, value.right, value.bottom };
	MapWindowPoints(handle, HWND_DESKTOP, reinterpret_cast<POINT *>(&r), (sizeof(RECT) / sizeof(POINT)));

	return m_rect_type{ r.left, r.top, r.right, r.bottom };
}

void winp::ui::window_surface::set_visible_state_(bool state){
	auto handle = get_handle_();
	if (handle == nullptr){
		if (state)
			add_styles_(WS_VISIBLE, false);
		else
			remove_styles_(WS_VISIBLE, false);
	}
	else if ((IsWindowVisible(handle) == FALSE) == state)
		ShowWindow(handle, (state ? SW_SHOW : SW_HIDE));
}

bool winp::ui::window_surface::get_visible_state_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_VISIBLE, false);
	return (IsWindowVisible(handle) != FALSE);
}

void winp::ui::window_surface::set_transaprent_state_(bool state){
	if (state)
		add_styles_(WS_EX_TRANSPARENT, true);
	else
		remove_styles_(WS_EX_TRANSPARENT, true);
}

bool winp::ui::window_surface::get_transaprent_state_() const{
	return has_styles_(WS_EX_TRANSPARENT, true);
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

void winp::ui::window_surface::create_(){
	if (get_handle_() != nullptr)
		return;//Already created

	auto parent_handle = get_first_window_ancestor_handle_();
	if (parent_handle == nullptr && get_parent_() != nullptr){
		throw_(error_value_type::window_parent_not_created);
		return;
	}

	auto styles = (styles_ | get_persistent_styles_());
	auto extended_styles = (extended_styles_ | get_persistent_extended_styles_());

	owner_->windows_manager_.cache_.handle = nullptr;
	owner_->windows_manager_.cache_.object = this;

	auto offset_from_window_ancestor = get_offset_from_ancestor_of_<window_surface>(m_point_type{});
	CreateWindowExW(
		extended_styles,
		get_class_name_(),
		get_window_text_(),
		styles,
		(position_.x + offset_from_window_ancestor.x),
		(position_.y + offset_from_window_ancestor.y),
		size_.width,
		size_.height,
		parent_handle,
		nullptr,
		get_instance_(),
		this
	);
}

void winp::ui::window_surface::set_maximized_state_(bool state){
	auto handle = get_handle_();
	if (handle == nullptr){
		if (state)
			add_styles_(WS_MAXIMIZE, false);
		else
			remove_styles_(WS_MAXIMIZE, false);
	}
	else if ((IsZoomed(handle) == FALSE) == state)
		ShowWindow(handle, (state ? SW_MAXIMIZE : SW_RESTORE));
}

bool winp::ui::window_surface::get_maximized_state_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_MAXIMIZE, false);
	return (IsZoomed(handle) != FALSE);
}

void winp::ui::window_surface::set_minimized_state_(bool state){
	auto handle = get_handle_();
	if (handle == nullptr){
		if (state)
			add_styles_(WS_MINIMIZE, false);
		else
			remove_styles_(WS_MINIMIZE, false);
	}
	else if ((IsIconic(handle) == FALSE) == state)
		ShowWindow(handle, (state ? SW_MINIMIZE : SW_RESTORE));
}

bool winp::ui::window_surface::get_minimized_state_() const{
	auto handle = get_handle_();
	if (handle == nullptr)
		return has_styles_(WS_MINIMIZE, false);
	return (IsIconic(handle) != FALSE);
}

void winp::ui::window_surface::set_styles_(DWORD value, bool is_extended){
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
}

void winp::ui::window_surface::add_styles_(DWORD value, bool is_extended){
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
		set_styles_((*target | value), is_extended);
}

void winp::ui::window_surface::remove_styles_(DWORD value, bool is_extended){
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
		set_styles_((*target & ~value), is_extended);
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
