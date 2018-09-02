#include "../app/app_object.h"

winp::message::object::object(thread::object &thread, ui::object *target)
	: thread_(&thread), owner_(target), target_(target), state_(state_type::nil){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t index){
		if (&prop == &do_default){
			if (*static_cast<const bool *>(value) && (state_ & (state_type::default_done | state_type::default_prevented)) == 0u)
				do_default_();
		}
		else if (&prop == &prevent_default)
			state_ = (*static_cast<const bool *>(value) ? (state_ | state_type::default_prevented) : state_);
		else if (&prop == &stop_propagation)
			state_ = (*static_cast<const bool *>(value) ? (state_ | state_type::propagation_stopped) : state_);
		else if (&prop == &stop_event_propagation)
			state_ = (*static_cast<const bool *>(value) ? (state_ | state_type::event_propagation_stopped) : state_);
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t index){
		if (&prop == &this->thread)
			*static_cast<thread::object **>(buf) = thread_;
		else if (&prop == &owner)
			*static_cast<ui::object **>(buf) = owner_;
		else if (&prop == &this->target)
			*static_cast<ui::object **>(buf) = target_;
	};

	this->thread.init_(nullptr, nullptr, getter);
	owner.init_(nullptr, nullptr, getter);
	this->target.init_(nullptr, nullptr, getter);

	do_default.init_(nullptr, setter, nullptr);
	prevent_default.init_(nullptr, setter, nullptr);

	stop_propagation.init_(nullptr, setter, nullptr);
	stop_event_propagation.init_(nullptr, setter, nullptr);
}

winp::message::object::~object() = default;

bool winp::message::object::bubble_(){
	if ((state_ & state_type::propagation_stopped) != 0u || static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(owner_) >> 32) == 0u)
		return false;//Not a managed object

	auto tree = dynamic_cast<ui::tree *>(owner_);
	if (tree == nullptr || tree->parent == nullptr)
		return false;

	owner_ = tree->parent;
	return true;
}

void winp::message::object::do_default_(){
	state_ |= state_type::default_done;
}

winp::message::basic::basic(thread::object &thread, ui::object *target, const info_type &info, m_default_callback_type default_callback)
	: object(thread, target), info_(info), result_(0), default_callback_(default_callback){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t index){
		if (&prop == &result){
			state_ |= state_type::result_set;
			result_ = *static_cast<const LRESULT *>(value);
		}
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t index){
		if (&prop == &this->info)
			*static_cast<info_type **>(buf) = &info_;
		else if (&prop == &result)
			*static_cast<LRESULT *>(buf) = result_;
	};

	this->info.init_(nullptr, nullptr, getter);
	result.init_(nullptr, setter, getter);
}

winp::message::basic::~basic() = default;

void winp::message::basic::do_default_(){

}
