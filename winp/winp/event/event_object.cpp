#include "../app/app_object.h"

winp::event::object::object(thread::object &thread, const callback_type &default_handler, const info_type &info)
	: target_(nullptr), context_(nullptr), thread_(thread), state_(state_type::nil), default_handler_(default_handler), result_(0), info_(info){}

winp::event::object::object(ui::object &target, const callback_type &default_handler, const info_type &info)
	: target_(&target), context_(&target), thread_(*target.thread_), state_(state_type::nil), default_handler_(default_handler), result_(0), info_(info){}

winp::event::object::object(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info)
	: target_(&target), context_(&context), thread_(*target.thread_), state_(state_type::nil), default_handler_(default_handler), result_(0), info_(info){}

winp::event::object::~object() = default;

winp::ui::object *winp::event::object::get_target() const{
	return (thread_.is_thread_context() ? target_: nullptr);
}

winp::ui::object *winp::event::object::get_context() const{
	return (thread_.is_thread_context() ? context_ : nullptr);
}

const winp::event::object::info_type *winp::event::object::get_info() const{
	return (thread_.is_thread_context() ? &info_ : nullptr);
}

void winp::event::object::set_result(LRESULT value, bool always_set){
	if (thread_.is_thread_context())
		set_result_(value, always_set);
}

void winp::event::object::set_result(bool value, bool always_set){
	set_result((value ? TRUE : FALSE), always_set);
}

LRESULT winp::event::object::get_result() const{
	return (thread_.is_thread_context() ? get_result_() : LRESULT());
}

void winp::event::object::prevent_default(){
	if (thread_.is_thread_context())
		state_ |= state_type::default_prevented;
}

void winp::event::object::do_default(){
	if (thread_.is_thread_context() && (state_ & state_type::default_prevented) == 0u){
		state_ |= state_type::default_prevented;
		do_default_();
	}
}

void winp::event::object::stop_propagation(){
	if (thread_.is_thread_context())
		state_ |= state_type::propagation_stopped;
}

void winp::event::object::set_result_(LRESULT value, bool always_set){
	if (always_set || (state_ & object::state_type::result_set) == 0u){
		result_ = value;
		state_ |= object::state_type::result_set;
	}
}

LRESULT winp::event::object::get_result_() const{
	return result_;
}

bool winp::event::object::bubble_(){
	return (((state_ & state_type::propagation_stopped) == 0u) && (context_ != nullptr) && (context_ = context_->get_parent_()) != nullptr);
}

void winp::event::object::do_default_(){
	if (default_handler_ != nullptr && !default_prevented_()){
		default_handler_(*this);
		state_ |= state_type::default_prevented;
	}
}

bool winp::event::object::default_prevented_() const{
	return ((state_ & state_type::default_prevented) != 0u);
}

bool winp::event::object::propagation_stopped_() const{
	return ((state_ & state_type::propagation_stopped) != 0u);
}

bool winp::event::object::result_set_() const{
	return ((state_ & state_type::result_set) != 0u);
}

winp::event::draw::draw(ui::object &target, const callback_type &default_handler, const info_type &info)
	: object(target, default_handler, info){}

winp::event::draw::draw(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info)
	: object(target, context, default_handler, info){}

winp::event::draw::~draw(){
	if (drawer_ != nullptr)
		drawer_->EndDraw();

	if (struct_.hdc != nullptr)
		RestoreDC(struct_.hdc, initial_device_state_id_);

	if (cleaner_ != nullptr)
		cleaner_();

	struct_ = PAINTSTRUCT{};
}

ID2D1RenderTarget *winp::event::draw::get_drawer(){
	return (thread_.is_thread_context() ? get_drawer_() : nullptr);
}

ID2D1SolidColorBrush *winp::event::draw::get_color_brush(){
	return (thread_.is_thread_context() ? get_color_brush_() : nullptr);
}

HDC winp::event::draw::get_device(){
	return (thread_.is_thread_context() ? get_device_() : nullptr);
}

winp::event::draw::m_rect_type winp::event::draw::get_region(){
	return (thread_.is_thread_context() ? get_region_() : m_rect_type{});
}

bool winp::event::draw::erase_background(){
	return (thread_.is_thread_context() ? erase_background_() : false);
}

void winp::event::draw::set_target_(ui::object *target, POINT &offset){
	auto surface_target = dynamic_cast<ui::surface *>(target);
	if (surface_target == nullptr || target == context_)
		return;//Do nothing

	auto poffset = surface_target->get_position_();
	{//Update offset
		offset.x += poffset.x;
		offset.y += poffset.y;
	}

	target_ = target;
	current_offset_ = offset;
	drawer_ = nullptr;

	if (struct_.hdc != nullptr){//Reset device
		RestoreDC(struct_.hdc, initial_device_state_id_);
		initial_device_state_id_ = SaveDC(struct_.hdc);
	}
}

void winp::event::draw::begin_(){
	if (struct_.hdc != nullptr || dynamic_cast<ui::surface *>(target_) == nullptr)
		return;//Already initialized

	switch (info_.code){
	case WM_PAINT:
		BeginPaint(target_->get_first_ancestor_of_<ui::window_surface>()->get_handle_(), &struct_);
		cleaner_ = [this]{
			EndPaint(target_->get_first_ancestor_of_<ui::window_surface>()->get_handle_(), &struct_);
		};
		break;
	case WM_PRINTCLIENT:
		struct_.hdc = reinterpret_cast<HDC>(info_.wparam);
		GetClipBox(struct_.hdc, &struct_.rcPaint);
		struct_.fErase = ((info_.lparam & PRF_ERASEBKGND) == PRF_ERASEBKGND);
		break;
	case WM_ERASEBKGND:
		struct_.hdc = reinterpret_cast<HDC>(info_.wparam);
		GetClipBox(struct_.hdc, &struct_.rcPaint);
		struct_.fErase = TRUE;
		break;
	default:
		break;
	}

	if (struct_.hdc != nullptr){//Initialize
		state_ |= state_type::default_prevented;
		initial_device_state_id_ = SaveDC(struct_.hdc);
	}
}

ID2D1RenderTarget *winp::event::draw::get_drawer_(){
	auto device = get_device_();
	if (device != nullptr && drawer_ == nullptr && (drawer_ = target_->thread_->get_device_drawer()) != nullptr){
		auto target_rect = dynamic_cast<ui::surface *>(target_)->get_client_dimension_();

		OffsetRect(&target_rect, current_offset_.x, current_offset_.y);
		OffsetClipRgn(struct_.hdc, current_offset_.x, current_offset_.y);
		IntersectClipRect(struct_.hdc, current_offset_.x, current_offset_.y, (target_rect.right + current_offset_.x), (target_rect.bottom + current_offset_.y));

		drawer_->BindDC(device, &target_rect);
		drawer_->SetTransform(D2D1::IdentityMatrix());
		drawer_->BeginDraw();
	}

	return drawer_;
}

ID2D1SolidColorBrush *winp::event::draw::get_color_brush_(){
	auto render = get_drawer_();
	if (render != nullptr && color_brush_ == nullptr && (color_brush_ = target_->thread_->get_color_brush()) != nullptr)
		color_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	return color_brush_;
}

HDC winp::event::draw::get_device_(){
	begin_();
	return struct_.hdc;
}

winp::event::draw::m_rect_type winp::event::draw::get_region_(){
	begin_();
	return struct_.rcPaint;
}

bool winp::event::draw::erase_background_(){
	begin_();
	return (struct_.fErase != FALSE);
}

winp::event::cursor::cursor(ui::object &target, const callback_type &default_handler, const info_type &info)
	: object(target, default_handler, info){}

winp::event::cursor::cursor(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info)
	: object(target, context, default_handler, info){}

winp::event::cursor::~cursor() = default;

WORD winp::event::cursor::get_hit_target() const{
	return (thread_.is_thread_context() ? LOWORD(info_.lparam) : 0);
}

WORD winp::event::cursor::get_mouse_button() const{
	return (thread_.is_thread_context() ? HIWORD(info_.lparam) : 0);
}

winp::event::mouse::mouse(ui::object &target, const callback_type &default_handler, const info_type &info, const m_point_type &offset, button_type button)
	: object(target, default_handler, info), offset_(offset), button_(button){}

winp::event::mouse::mouse(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info, const m_point_type &offset, button_type button)
	: object(target, context, default_handler, info), offset_(offset), button_(button){}

winp::event::mouse::~mouse() = default;

winp::event::mouse::m_point_type winp::event::mouse::get_position() const{
	return (thread_.is_thread_context() ? get_position_() : m_point_type{});
}

winp::event::mouse::m_point_type winp::event::mouse::get_offset() const{
	return (thread_.is_thread_context() ? offset_ : m_point_type{});
}

winp::event::mouse::button_type winp::event::mouse::get_button() const{
	return (thread_.is_thread_context() ? button_ : button_type::nil);
}

winp::event::mouse::m_point_type winp::event::mouse::get_position_() const{
	if (!thread_.is_thread_context())
		return m_point_type{};

	auto position = ::GetMessagePos();
	return m_point_type{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
}

winp::event::key::keyboard_state::keyboard_state(key &e)
	: e_(e){}

bool winp::event::key::keyboard_state::check_state(BYTE key) const{
	if (!e_.thread_.is_thread_context())
		return false;

	retrieve_states_();

	if (key == VK_CAPITAL || key == VK_NUMLOCK || key == VK_SCROLL || key == VK_INSERT)
		return ((key::keyboard_states_[key] & 1u) != 0u);

	return (key::keyboard_states_[key] < 0);
}

bool winp::event::key::keyboard_state::left_shift_pressed() const{
	return check_state(VK_LSHIFT);
}

bool winp::event::key::keyboard_state::right_shift_pressed() const{
	return check_state(VK_RSHIFT);
}

bool winp::event::key::keyboard_state::shift_pressed() const{
	return check_state(VK_SHIFT);
}

bool winp::event::key::keyboard_state::left_ctrl_pressed() const{
	return check_state(VK_LCONTROL);
}

bool winp::event::key::keyboard_state::right_ctrl_pressed() const{
	return check_state(VK_RCONTROL);
}

bool winp::event::key::keyboard_state::ctrl_pressed() const{
	return check_state(VK_CONTROL);
}

bool winp::event::key::keyboard_state::left_alt_pressed() const{
	return check_state(VK_LMENU);
}

bool winp::event::key::keyboard_state::right_alt_pressed() const{
	return check_state(VK_RMENU);
}

bool winp::event::key::keyboard_state::alt_pressed() const{
	return check_state(VK_MENU);
}

bool winp::event::key::keyboard_state::left_win_pressed() const{
	return check_state(VK_LWIN);
}

bool winp::event::key::keyboard_state::right_win_pressed() const{
	return check_state(VK_RWIN);
}

bool winp::event::key::keyboard_state::win_pressed() const{
	return (left_win_pressed() || right_win_pressed());
}

bool winp::event::key::keyboard_state::caps_lock_on() const{
	return check_state(VK_CAPITAL);
}

bool winp::event::key::keyboard_state::num_lock_on() const{
	return check_state(VK_NUMLOCK);
}

bool winp::event::key::keyboard_state::scroll_lock_on() const{
	return check_state(VK_SCROLL);
}

bool winp::event::key::keyboard_state::insert_on() const{
	return check_state(VK_INSERT);
}

void winp::event::key::keyboard_state::retrieve_states_() const{
	if (!key::keyboard_states_retrieved_)
		key::keyboard_states_retrieved_ = (GetKeyboardState(key::keyboard_states_) != FALSE);
}

winp::event::key::key(ui::object &target, const callback_type &default_handler, const info_type &info)
	: object(target, default_handler, info), keyboard_state_(*this){}

winp::event::key::key(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info)
	: object(target, context, default_handler, info), keyboard_state_(*this){}

winp::event::key::~key() = default;

unsigned short winp::event::key::get_code() const{
	return (thread_.is_thread_context() ? static_cast<unsigned short>(info_.wparam) : 0u);
}

wchar_t winp::event::key::get_char() const{
	return (thread_.is_thread_context() ? (reinterpret_cast<const wchar_t *>(&info_.lparam))[2] : L'\0');
}

WORD winp::event::key::get_repeat_count() const{
	return (is_down() ? static_cast<WORD>(info_.lparam) : 0);
}

bool winp::event::key::is_char() const{
	return (thread_.is_thread_context() && info_.code == WM_CHAR);
}

bool winp::event::key::is_down() const{
	return (thread_.is_thread_context() && info_.code == WM_KEYDOWN);
}

bool winp::event::key::is_first_down() const{
	return (is_down() ? std::bitset<sizeof(LPARAM) * 8>(info_.lparam).test(30) : false);
}

bool winp::event::key::is_being_released() const{
	return (is_char() ? std::bitset<sizeof(LPARAM) * 8>(info_.lparam).test(31) : false);
}

bool winp::event::key::is_extended() const{
	return (thread_.is_thread_context() && std::bitset<sizeof(LPARAM) * 8>(info_.lparam).test(24));
}

const winp::event::key::keyboard_state &winp::event::key::get_keyboard_state() const{
	return keyboard_state_;
}

thread_local bool winp::event::key::keyboard_states_retrieved_ = false;
thread_local BYTE winp::event::key::keyboard_states_[0x100];
