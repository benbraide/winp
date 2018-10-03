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
