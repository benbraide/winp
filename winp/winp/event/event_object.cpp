#include "../app/app_object.h"

winp::event::object::object(ui::object *target)
	: owner_(target), target_(target), state_(state_type::nil){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
		if (&prop == &prevent_default)
			state_ = (*static_cast<const bool *>(value) ? (state_ | state_type::default_prevented) : state_);
		else if (&prop == &stop_propagation)
			state_ = (*static_cast<const bool *>(value) ? (state_ | state_type::propagation_stopped) : state_);
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &owner)
			*static_cast<ui::object **>(buf) = owner_;
		else if (&prop == &this->target)
			*static_cast<ui::object **>(buf) = target_;
		else if (&prop == &prevent_default)
			*static_cast<bool *>(buf) = ((state_ & state_type::default_prevented) != 0u);
		else if (&prop == &stop_propagation)
			*static_cast<bool *>(buf) = ((state_ & state_type::propagation_stopped) != 0u);
	};

	owner.init_(nullptr, nullptr, getter);
	this->target.init_(nullptr, nullptr, getter);

	prevent_default.init_(nullptr, setter, getter);
	stop_propagation.init_(nullptr, setter, getter);
}

winp::event::object::~object() = default;

bool winp::event::object::bubble_(){
	return (((state_ & state_type::propagation_stopped) == 0u) && (owner_ != nullptr) && (owner_ = owner_->parent) != nullptr);
}

winp::event::message::message(ui::object *target, const info_type &info)
	: object(target), info_(info){}

winp::event::message::~message() = default;

LRESULT winp::event::message::get_result_() const{
	return LRESULT();
}

winp::event::draw::draw(ui::object *target, const info_type &info)
	: message(target, info){
	init_();
}

winp::event::draw::~draw(){
	if (struct_.hdc != nullptr)
		RestoreDC(struct_.hdc, initial_device_state_id_);

	if (cleaner_ != nullptr)
		cleaner_();

	struct_ = PAINTSTRUCT{};
}

void winp::event::draw::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &drawer)
			*static_cast<ID2D1RenderTarget **>(buf) = get_drawer_();
		else if (&prop == &color_brush)
			*static_cast<ID2D1SolidColorBrush **>(buf) = get_color_brush_();
		else if (&prop == &device)
			*static_cast<HDC *>(buf) = get_device_();
		else if (&prop == &region)
			*static_cast<m_rect_type *>(buf) = get_region_();
		else if (&prop == &erase_background)
			*static_cast<bool *>(buf) = erase_background_();
	};

	drawer.init_(nullptr, nullptr, getter);
	color_brush.init_(nullptr, nullptr, getter);

	device.init_(nullptr, nullptr, getter);
	region.init_(nullptr, nullptr, getter);

	erase_background.init_(nullptr, nullptr, getter);
}

void winp::event::draw::set_target_(ui::object *target, utility::point<int> &offset){
	auto surface_target = dynamic_cast<ui::surface *>(target);
	if (surface_target == nullptr || target == owner_)
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
	if (device != nullptr && drawer_ == nullptr && (drawer_ = target_->owner_->get_device_drawer_()) != nullptr){
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
	if (render != nullptr && color_brush_ == nullptr && (color_brush_ = target_->owner_->get_color_brush_()) != nullptr)
		color_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	return color_brush_;
}

HDC winp::event::draw::get_device_(){
	begin_();
	return struct_.hdc;
}

winp::event::draw::m_rect_type winp::event::draw::get_region_(){
	begin_();
	return m_rect_type{ struct_.rcPaint.left, struct_.rcPaint.top, struct_.rcPaint.right, struct_.rcPaint.bottom };
}

bool winp::event::draw::erase_background_(){
	begin_();
	return (struct_.fErase != FALSE);
}

winp::event::mouse::mouse(ui::object *target, const info_type &info, const m_point_type &offset, button_type button)
	: message(target, info), offset_(offset), button_(button){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &position)
			*static_cast<m_point_type *>(buf) = get_position_();
		else if (&prop == &this->offset)
			*static_cast<m_point_type *>(buf) = offset_;
		else if (&prop == &this->button)
			*static_cast<button_type *>(buf) = button_;
	};

	position.init_(nullptr, nullptr, getter);
	this->offset.init_(nullptr, nullptr, getter);
	this->button.init_(nullptr, nullptr, getter);
}

winp::event::mouse::~mouse() = default;

winp::event::mouse::m_point_type winp::event::mouse::get_position_() const{
	auto position = ::GetMessagePos();
	return m_point_type{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
}
