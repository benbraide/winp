#include "../app/app_object.h"

winp::control::object::object() = default;

winp::control::object::object(thread::object &thread)
	: window_surface(thread){}

winp::control::object::~object() = default;

bool winp::control::object::set_font(HFONT value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_font_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_font_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

HFONT winp::control::object::get_font(const std::function<void(HFONT)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_font_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_font_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_font_(); }, thread::queue::send_priority, id_);
}

bool winp::control::object::set_text(const std::wstring &value, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_text_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_text_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

std::wstring winp::control::object::get_text(const std::function<void(const std::wstring &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_text_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_text_()); }, thread::queue::send_priority, id_);
		return L"";
	}

	return thread_.queue.execute([this]{ return get_text_(); }, thread::queue::send_priority, id_);
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
	if (GetTextExtentPoint32W(device, text.data(), static_cast<int>(text.size()), &computed_size) == FALSE){
		SelectObject(device, old_font);//Restore font
		return m_size_type{};
	}

	SelectObject(device, old_font);//Restore font
	return m_size_type{ computed_size.cx, symbol_list_size.cy };
}

void winp::control::object::add_to_toplevel_(bool update){}

void winp::control::object::post_create_(){
	if (font_ != nullptr)//Update font
		SendMessageW(static_cast<HWND>(get_handle_()), WM_SETFONT, reinterpret_cast<WPARAM>(font_), 0);
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
	return false;
}

HFONT winp::control::object::get_font_() const{
	return nullptr;
}

bool winp::control::object::set_text_(const std::wstring &value){
	return false;
}

std::wstring winp::control::object::get_text_() const{
	return L"";
}

void winp::control::object::update_size_(){
	auto size = compute_size_();
	auto additional_size = compute_additional_size_();
	set_size_(m_size_type{ (size.cx + additional_size.cx), (size.cy + additional_size.cy) });
}

winp::ui::surface::m_size_type winp::control::object::compute_size_() const{
	auto handle = static_cast<HWND>(get_handle_());
	if (handle == nullptr)//Object not created
		return m_size_type{};

	auto device = GetDC(handle);
	if (device == nullptr)//Failed to retrieve device
		return m_size_type{};

	auto size = compute_size(handle, device, get_font_(), get_text_());
	ReleaseDC(handle, device);

	return size;
}

winp::ui::surface::m_size_type winp::control::object::compute_additional_size_() const{
	return m_size_type{};
}
