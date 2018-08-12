#include "../thread/current_thread.h"

winp::message::object::object(thread::object &thread_owner, m_item_type *target)
	: thread_owner_(&thread_owner), owner_(target), target_(target), state_(state_type::nil){
	auto setter = [this](const prop::base<object> &prop, const void *value, std::size_t index){
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

	auto getter = [this](const prop::base<object> &prop, void *buf, std::size_t index){
		if (&prop == &owner)
			*static_cast<m_item_type **>(buf) = owner_;
		else if (&prop == &this->target)
			*static_cast<m_item_type **>(buf) = target_;
	};

	owner.init_(*this, nullptr, nullptr, getter, &error);
	this->target.init_(*this, nullptr, nullptr, getter, &error);

	do_default.init_(*this, nullptr, setter, nullptr, &error);
	prevent_default.init_(*this, nullptr, setter, nullptr, &error);

	stop_propagation.init_(*this, nullptr, setter, nullptr, &error);
	stop_event_propagation.init_(*this, nullptr, setter, nullptr, &error);
}

winp::message::object::~object() = default;

bool winp::message::object::bubble_(){
	if (static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(owner_) >> 32) == 0u)
		return false;//Not a managed object

	//#TODO: Cast to tree item
	//#TODO: Set owner_ to parent

	return true;
}

void winp::message::object::do_default_(){
	state_ |= state_type::default_done;
}

void winp::message::object::pop_managed_(m_item_type *&receiver, unsigned int &msg){
	auto info = thread_owner_->pop_managed_();
	receiver = info.receiver;
	msg = info.msg;
}

std::shared_ptr<winp::message::object::m_thread_value_type> winp::message::object::pop_value_(unsigned __int64 key){
	return thread_owner_->pop_value_(key);
}

winp::message::basic::basic(thread::object &thread_owner, m_item_type *target, const info_type &info, m_default_callback_type default_callback)
	: object(thread_owner, target), info_(info), wparam_(nullptr), lparam_(nullptr), result_(0), default_callback_(default_callback){
	if (target_ == thread::object::item_placeholders.allocated_wparam_only.get() && target_ == thread::object::item_placeholders.allocated_both.get()){
		update_managed_();
		update_wparam_();
		update_lparam_();
	}
	else if (target_ == thread::object::item_placeholders.allocated_wparam_only.get()){
		update_managed_();
		update_wparam_();
	}
	else if (target_ == thread::object::item_placeholders.allocated_lparam_only.get()){
		update_managed_();
		update_lparam_();
	}
}


winp::message::basic::~basic() = default;

void winp::message::basic::do_default_(){

}

void winp::message::basic::update_managed_(){
	pop_managed_(target_, info_.code);
	owner_ = target_;
}

void winp::message::basic::update_wparam_(){
	wparam_ = pop_value_(info_.wparam);
	info_.wparam = wparam_->get<WPARAM>();
}

void winp::message::basic::update_lparam_(){
	lparam_ = pop_value_(info_.lparam);
	info_.lparam = lparam_->get<LPARAM>();
}
