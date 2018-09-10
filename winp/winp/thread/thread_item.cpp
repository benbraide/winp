#include "thread_object.h"

winp::thread::item_message::item_message() = default;

winp::thread::item_message::item_message(const item_message &copy)
	: target_(copy.target_){
	if (target_ != nullptr)
		init_();
}

winp::thread::item_message &winp::thread::item_message::operator=(const item_message &copy){
	if ((target_ = copy.target_) != nullptr && target.getter_ == nullptr)
		init_();
	return *this;
}

void winp::thread::item_message::init_(){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
		if (&prop == &action){
			switch (*static_cast<const action_type *>(value)){
			case action_type::send:
				send();
				break;
			case action_type::post:
				post();
				break;
			default:
				break;
			}
		}
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &target)
			*static_cast<HWND *>(buf) = target_;
		else if (&prop == &result)
			*static_cast<LRESULT *>(buf) = result_;
	};

	target.init_(nullptr, nullptr, getter);
	result.init_(nullptr, nullptr, getter);
	action.init_(nullptr, setter, nullptr);
}

LRESULT winp::thread::item_message::send(){
	if (static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(target_) >> 32) != 0u){
		auto item_target = reinterpret_cast<item *>(target_);
		result_ = item_target->owner_->queue->add([item_target]{
			return LRESULT();
		}, 4).get();
	}
	else//Target is a native handle
		result_ = SendMessageW(target_, code.m_value_, wparam.m_value_, lparam.m_value_);

	return result_;
}

bool winp::thread::item_message::post(){
	if (static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(target_) >> 32) != 0u){

	}
	else//Target is a native handle
		result_ = PostMessageW(target_, code.m_value_, wparam.m_value_, lparam.m_value_);

	return (result_ != FALSE);
}

winp::thread::item::item()
	: owner_(nullptr), handle_(reinterpret_cast<HWND>(this)){
	init_();
}

winp::thread::item::item(object &owner)
	: owner_(&owner), handle_(reinterpret_cast<HWND>(this)){
	init_();
}

winp::thread::item::~item() = default;

void winp::thread::item::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &thread)
			*static_cast<object **>(buf) = owner_;
		else if (&prop == &handle)
			*static_cast<HWND *>(buf) = get_handle_();
		else if (&prop == &request)
			do_request_(buf, *reinterpret_cast<std::type_info *>(context));
	};

	thread.init_(nullptr, nullptr, getter);
	handle.init_(nullptr, nullptr, getter);
	request.init_(nullptr, nullptr, getter);
}

void winp::thread::item::destroy_(){}

void winp::thread::item::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(item_message)){
		static_cast<item_message *>(buf)->target_ = get_handle_();
		static_cast<item_message *>(buf)->init_();
	}
}

HWND winp::thread::item::get_handle_() const{
	return handle_;
}
