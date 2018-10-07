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

winp::event::drawing::drawing(ui::object *target, const info_type &info)
	: message(target, info){
	init_();
}

winp::event::drawing::~drawing(){
	if (struct_.hdc != nullptr)
		RestoreDC(struct_.hdc, -1);

	if (cleaner_ != nullptr)
		cleaner_();

	struct_ = PAINTSTRUCT{};
}

void winp::event::drawing::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &render)
			*static_cast<ID2D1RenderTarget **>(buf) = get_render_();
		else if (&prop == &color_brush)
			*static_cast<ID2D1SolidColorBrush **>(buf) = get_color_brush_();
		else if (&prop == &device)
			*static_cast<HDC *>(buf) = get_device_();
		else if (&prop == &region)
			*static_cast<m_rect_type *>(buf) = get_region_();
		else if (&prop == &erase_background)
			*static_cast<bool *>(buf) = erase_background_();
	};

	render.init_(nullptr, nullptr, getter);
	color_brush.init_(nullptr, nullptr, getter);

	device.init_(nullptr, nullptr, getter);
	region.init_(nullptr, nullptr, getter);

	erase_background.init_(nullptr, nullptr, getter);
}

void winp::event::drawing::set_target_(ui::object *target){
	begin_();
	if ((target_ = target) != nullptr){//Align drawing region
		RestoreDC(struct_.hdc, -1);

		utility::point<int> offset, total_offset{ 0, 0 };
		for (; target != owner_; target = target->get_parent_()){//Compute offset
			offset = dynamic_cast<ui::surface *>(target)->get_position_();
			total_offset.x += offset.x;
			total_offset.y += offset.y;
		}

		SaveDC(struct_.hdc);
	}
}

void winp::event::drawing::begin_(){
	if (struct_.hdc != nullptr)
		return;//Already initialized

	switch (info_.code){
	case WM_PAINT:
		BeginPaint(target_->get_first_ancestor_of_<ui::window_surface>()->get_handle_(), &struct_);
		cleaner_ = [this]{
			EndPaint(target_->get_first_ancestor_of_<ui::window_surface>()->get_handle_(), &struct_);
			state_ |= state_type::default_prevented;
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
}

ID2D1RenderTarget *winp::event::drawing::get_render_(){
	auto device = get_device_();
	if (device != nullptr && render_ == nullptr && (render_ = target_->owner_->get_device_render_()) != nullptr){
		dynamic_cast<ID2D1DCRenderTarget *>(render_)->BindDC(device, &struct_.rcPaint);
		render_->SetTransform(D2D1::IdentityMatrix());
	}

	return render_;
}

ID2D1SolidColorBrush *winp::event::drawing::get_color_brush_(){
	auto render = get_render_();
	if (render != nullptr && color_brush_ == nullptr && (color_brush_ = target_->owner_->get_color_brush_()) != nullptr)
		color_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	return color_brush_;
}

HDC winp::event::drawing::get_device_(){
	begin_();
	return struct_.hdc;
}

winp::event::drawing::m_rect_type winp::event::drawing::get_region_(){
	begin_();
	return m_rect_type{ struct_.rcPaint.left, struct_.rcPaint.top, struct_.rcPaint.right, struct_.rcPaint.bottom };
}

bool winp::event::drawing::erase_background_(){
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
