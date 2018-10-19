#include "../app/app_object.h"

winp::event::object::object(thread::object &thread)
	: target_(nullptr), context_(nullptr), thread_(thread), state_(state_type::nil){}

winp::event::object::object(ui::object &target)
	: target_(&target), context_(&target), thread_(*target.thread_), state_(state_type::nil){}

winp::event::object::object(ui::object &target, ui::object &context)
	: target_(&target), context_(&context), thread_(*target.thread_), state_(state_type::nil){}

winp::event::object::~object() = default;

winp::ui::object *winp::event::object::get_target() const{
	return (thread_.is_thread_context() ? target_: nullptr);
}

winp::ui::object *winp::event::object::get_context() const{
	return (thread_.is_thread_context() ? context_ : nullptr);
}

void winp::event::object::set_result(LRESULT value){
	if (thread_.is_thread_context())
		set_result_(value);
}

void winp::event::object::set_result(bool value){
	set_result(value ? TRUE : FALSE);
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

void winp::event::object::set_result_(LRESULT value){}

LRESULT winp::event::object::get_result_() const{
	return LRESULT();
}

bool winp::event::object::bubble_(){
	return (((state_ & state_type::propagation_stopped) == 0u) && (context_ != nullptr) && (context_ = context_->get_parent_()) != nullptr);
}

void winp::event::object::do_default_(){}

bool winp::event::object::default_prevented_() const{
	return ((state_ & state_type::default_prevented) != 0u);
}

bool winp::event::object::propagation_stopped_() const{
	return ((state_ & state_type::propagation_stopped) != 0u);
}

bool winp::event::object::result_set_() const{
	return ((state_ & state_type::result_set) != 0u);
}

winp::event::message::message(ui::object &target, const info_type &info)
	: object(target), info_(info){}

winp::event::message::message(ui::object &target, ui::object &context, const info_type &info)
	: object(target, context), info_(info){}

winp::event::message::~message() = default;

winp::event::draw::draw(ui::object &target, const info_type &info)
	: message(target, info){}

winp::event::draw::draw(ui::object &target, ui::object &context, const info_type &info)
	: message(target, context, info){}

winp::event::draw::~draw(){
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
		auto target_rect = dynamic_cast<ui::surface *>(target_)->get_dimension_();
		RECT target_native_rect{
			target_rect.left,
			target_rect.top,
			target_rect.right,
			target_rect.bottom
		};

		OffsetRect(&target_native_rect, current_offset_.x, current_offset_.y);
		OffsetClipRgn(struct_.hdc, current_offset_.x, current_offset_.y);
		IntersectClipRect(struct_.hdc, current_offset_.x, current_offset_.y, (target_rect.right + current_offset_.x), (target_rect.bottom + current_offset_.y));

		dynamic_cast<ID2D1DCRenderTarget *>(drawer_)->BindDC(device, &target_native_rect);
		drawer_->SetTransform(D2D1::IdentityMatrix());
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

winp::event::mouse::mouse(ui::object &target, const info_type &info, const m_point_type &offset, button_type button)
	: message(target, info), offset_(offset), button_(button){}

winp::event::mouse::mouse(ui::object &target, ui::object &context, const info_type &info, const m_point_type &offset, button_type button)
	: message(target, context, info), offset_(offset), button_(button){}

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
	auto position = ::GetMessagePos();
	return m_point_type{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
}
