#include "../app/app_object.h"

winp::control::object::object(){
	styles_ |= WS_VISIBLE;
}

winp::control::object::object(thread::object &thread)
	: window_surface(thread){
	styles_ |= WS_VISIBLE;
}

winp::control::object::~object() = default;

bool winp::control::object::set_font(HFONT value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_font_(value));
	});
}

HFONT winp::control::object::get_font(const std::function<void(HFONT)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_font_());
	}, callback != nullptr);
}

bool winp::control::object::set_text(const std::wstring &value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_text_(value));
	});
}

std::wstring winp::control::object::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_text_());
	}, callback != nullptr);
}

winp::ui::surface::m_size_type winp::control::object::compute_size(HWND handle, HDC device, HFONT font, const std::wstring &text){
	static const wchar_t *symbol_list = L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";

	m_size_type symbol_list_size{};
	auto old_font = SelectObject(device, font);

	if (GetTextExtentPoint32W(device, symbol_list, static_cast<int>(std::wcslen(symbol_list)), &symbol_list_size) == FALSE){
		SelectObject(device, old_font);//Restore font
		return m_size_type{};
	}

	m_size_type computed_size{};
	if (!text.empty() && GetTextExtentPoint32W(device, text.data(), static_cast<int>(text.size()), &computed_size) == FALSE){
		SelectObject(device, old_font);//Restore font
		return m_size_type{};
	}

	SelectObject(device, old_font);//Restore font
	return m_size_type{ computed_size.cx, symbol_list_size.cy };
}

const wchar_t *winp::control::object::get_theme_name_() const{
	return get_class_name_();
}

void winp::control::object::add_to_toplevel_(bool update){}

void winp::control::object::post_create_(){
	if (font_ != nullptr)//Update font
		SendMessageW(static_cast<HWND>(get_handle_()), WM_SETFONT, reinterpret_cast<WPARAM>(font_), 0);
}

bool winp::control::object::set_padding_(const m_rect_type &value){
	if (!ui::io_surface::set_padding_(value))
		return false;

	padding_changed_();
	update_size_();

	return true;
}

HINSTANCE winp::control::object::get_instance_() const{
	return nullptr;
}

const wchar_t *winp::control::object::get_window_text_() const{
	return text_.data();
}

DWORD winp::control::object::get_filtered_styles_() const{
	return ~(WS_VISIBLE | WS_TABSTOP);
}

DWORD winp::control::object::get_filtered_extended_styles_() const{
	return (WS_EX_APPWINDOW | WS_EX_CONTEXTHELP | WS_EX_DLGMODALFRAME);
}

void winp::control::object::handle_unhandled_event_(event::object &e){
	e.do_default();
}

bool winp::control::object::set_font_(HFONT value){
	auto old_font = font_;
	font_ = value;

	font_changed_(old_font);
	update_size_();

	return true;
}

HFONT winp::control::object::get_font_() const{
	return font_;
}

void winp::control::object::font_changed_(HFONT old_font){
	if (auto handle = get_handle_(); handle != nullptr)//Update font
		SendMessageW(static_cast<HWND>(handle), WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
}

bool winp::control::object::set_text_(const std::wstring &value){
	text_ = value;
	if (auto handle = get_handle_(); handle == nullptr || SendMessageW(static_cast<HWND>(handle), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(value.data())) != FALSE)
		update_size_();
	return true;
}

const std::wstring &winp::control::object::get_text_() const{
	return text_;
}

void winp::control::object::padding_changed_(){}

void winp::control::object::update_size_(){
	set_size_(get_computed_size_());
}

winp::ui::surface::m_size_type winp::control::object::get_computed_size_() const{
	auto size = compute_size_();
	auto additional_size = compute_additional_size_(size);
	auto padding = get_padding_();

	return m_size_type{ (size.cx + additional_size.cx + padding.left + padding.right), (size.cy + additional_size.cy + padding.top + padding.bottom) };
}

winp::ui::surface::m_size_type winp::control::object::compute_size_() const{
	auto handle = static_cast<HWND>(get_handle_());
	auto device = GetDC(handle);

	if (device == nullptr)//Failed to retrieve device
		return m_size_type{};

	auto size = compute_size(handle, device, get_font_(), get_text_());
	ReleaseDC(handle, device);

	return size;
}

winp::ui::surface::m_size_type winp::control::object::compute_additional_size_(const m_size_type &size) const{
	return m_size_type{};
}

bool winp::control::object::is_uniform_padding_() const{
	return (padding_.left == padding_.right && padding_.top == padding_.bottom);
}
