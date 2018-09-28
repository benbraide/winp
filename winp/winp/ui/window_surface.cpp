#include "../app/app_object.h"

winp::ui::window_surface::window_surface(thread::object &thread)
	: visible_surface(thread){
	init_();
}

winp::ui::window_surface::window_surface(tree &parent)
	: visible_surface(thread){
	init_();
}

winp::ui::window_surface::~window_surface(){
	destroy_();
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
		else if (&prop == &background_color)
			set_background_color_(*static_cast<const m_rgba_type *>(value));
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &visible)
			*static_cast<bool *>(buf) = has_state_(state_visible);
		else if (&prop == &transparent)
			*static_cast<bool *>(buf) = has_state_(state_transparent);
		else if (&prop == &background_color)
			*static_cast<m_rgba_type *>(buf) = get_background_color_();
	};

	styles.init_(nullptr, setter, getter);
	extended_styles.init_(nullptr, setter, getter);

	created.init_(nullptr, setter, getter);
	maximized.init_(nullptr, setter, getter);
	minimized.init_(nullptr, setter, getter);

	create_event_.thread_ = owner_;
	destroy_event_.thread_ = owner_;

	handle_ = nullptr;
}

void winp::ui::window_surface::destroy_(){
	owner_->queue_.post([this]{
		if (handle_ != nullptr)
			DestroyWindow(handle_);
	}, thread::queue::send_priority);
}

void winp::ui::window_surface::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(create_event_type))
		*static_cast<create_event_type *>(buf) = create_event_type(create_event_);
	else if (id == typeid(destroy_event_type))
		*static_cast<destroy_event_type *>(buf) = destroy_event_type(destroy_event_);
	else if (id == typeid(window_surface *))
		*static_cast<window_surface **>(buf) = this;
	else
		visible_surface::do_request_(buf, id);
}

void winp::ui::window_surface::do_apply_(const void *value, const std::type_info &id){

}

HWND winp::ui::window_surface::get_handle_() const{
	return ((handle_ == nullptr) ? reinterpret_cast<HWND>(const_cast<window_surface *>(this)) : handle_);
}

void winp::ui::window_surface::set_size_(const m_size_type &value){
	if (handle_ == nullptr)
		visible_surface::set_size_(value);
	else
		::SetWindowPos(handle_, nullptr, 0, 0, value.width, value.height, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE));
}

winp::ui::surface::m_size_type winp::ui::window_surface::get_client_position_offset_() const{
	if (handle_ == nullptr)
		return visible_surface::get_client_position_offset_();

	POINT client_offset{ 0, 0 };
	if (::ClientToScreen(handle_, &client_offset) == FALSE)
		return visible_surface::get_client_position_offset_();

	RECT window_rect{};
	if (::GetWindowRect(handle_, &window_rect) == FALSE)
		return visible_surface::get_client_position_offset_();

	return m_size_type{ (client_offset.x - window_rect.left), (client_offset.y - window_rect.top) };
}

void winp::ui::window_surface::set_position_(const m_point_type &value){
	if (handle_ == nullptr)
		visible_surface::set_position_(value);
	else
		::SetWindowPos(handle_, nullptr, value.x, value.y, 0, 0, (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE));
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_from_absolute_value_(const m_point_type &value) const{
	POINT p{ value.x, value.y };
	if (handle_ != nullptr)
		::ScreenToClient(handle_, &p);
	return m_point_type{ p.x, p.y };
}

winp::ui::surface::m_point_type winp::ui::window_surface::convert_position_to_absolute_value_(const m_point_type &value) const{
	POINT p{ value.x, value.y };
	if (handle_ != nullptr)
		::ClientToScreen(handle_, &p);
	return m_point_type{ p.x, p.y };
}

winp::ui::window_surface *winp::ui::window_surface::get_window_surface_parent_() const{
	return dynamic_cast<window_surface *>(get_parent_());
}

void winp::ui::window_surface::set_handle_(HWND value){
	handle_ = value;
}

void winp::ui::window_surface::set_message_entry_(LONG_PTR value){}

void winp::ui::window_surface::add_to_toplevel_(){
	if (get_parent_() == nullptr)
		owner_->windows_manager_.toplevel_map_[get_handle_()] = this;
}

void winp::ui::window_surface::create_(){
	owner_->queue_.post([this]{
		if (handle_ != nullptr)
			return;

		auto styles = (styles_ | get_persistent_styles_());
		auto extended_styles = (extended_styles_ | get_persistent_extended_styles_());

		HWND parent_handle = nullptr;
		auto position = position_;
		if (!resolve_parent_(parent_handle, position, styles))
			return throw_(error_value_type::window_parent_not_created);

		owner_->windows_manager_.cache_.handle = nullptr;
		owner_->windows_manager_.cache_.object = this;

		CreateWindowExW(
			extended_styles,
			get_class_name_(),
			get_window_text_(),
			styles,
			position_.x,
			position_.y,
			size_.width,
			size_.height,
			parent_handle,
			nullptr,
			get_instance_(),
			this
		);
	}, thread::queue::send_priority);
}

void winp::ui::window_surface::set_styles_(DWORD value, bool is_extended){
	if (is_extended)
		extended_styles_ = (value & ~get_filtered_extended_styles_());
	else//Basic styles
		styles_ = (value & ~get_filtered_styles_());

	if (get_handle_() != nullptr){//Update window

	}
}

DWORD winp::ui::window_surface::get_styles_(bool is_extended) const{
	return (is_extended ? (extended_styles_ | get_persistent_extended_styles_()) : (styles_ | get_persistent_styles_()));
}

DWORD winp::ui::window_surface::get_persistent_styles_() const{
	return 0u;
}

DWORD winp::ui::window_surface::get_persistent_extended_styles_() const{
	return 0u;
}

DWORD winp::ui::window_surface::get_filtered_styles_() const{
	return WS_CHILD;
}

DWORD winp::ui::window_surface::get_filtered_extended_styles_() const{
	return 0u;
}

HINSTANCE winp::ui::window_surface::get_instance_() const{
	return GetModuleHandleW(nullptr);
}

WNDPROC winp::ui::window_surface::get_default_message_entry_() const{
	return DefWindowProcW;
}

const wchar_t *winp::ui::window_surface::get_class_name_() const{
	return WINP_CLASS_WUUID;
}

const wchar_t *winp::ui::window_surface::get_window_text_() const{
	return L"";
}

bool winp::ui::window_surface::resolve_parent_(HWND &handle, m_point_type &position, DWORD &styles) const{
	window_surface *window_parent;
	for (auto parent = get_parent_(); parent != nullptr; parent = parent->parent){
		if ((window_parent = dynamic_cast<window_surface *>(parent)) == nullptr)
			continue;//Ancestor is not a window surface

		if ((handle = window_parent->handle_) == nullptr)
			return false;//Window ancestor not created

		styles |= WS_CHILD;
	}

	return true;
}

void winp::ui::window_surface::fire_event_(m_event_type &ev, event::object &e) const{
	ev.fire_(e);
}
