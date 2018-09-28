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
