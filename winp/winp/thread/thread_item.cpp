#include "../app/app_object.h"

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
	auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
		if (&prop == &apply)
			do_apply_(value, *reinterpret_cast<std::type_info *>(context));
		else if (&prop == &queued_task)
			owner_->queue->post(*reinterpret_cast<queue::callback_type *>(context), queue::send_priority);
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &handle){
			owner_->queue_.add([&]{
				*static_cast<HWND *>(buf) = get_handle_();
			}, queue::send_priority).get();
		}
		else if (&prop == &thread)
			*static_cast<object **>(buf) = owner_;
		else if (&prop == &request)
			do_request_(buf, *reinterpret_cast<std::type_info *>(context));
	};

	thread.init_(nullptr, nullptr, getter);
	handle.init_(nullptr, nullptr, getter);

	request.init_(nullptr, nullptr, getter);
	apply.init_(nullptr, setter, nullptr);

	queued_task.init_(nullptr, setter, nullptr);
}

void winp::thread::item::destroy_(){}

void winp::thread::item::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(item *))
		*static_cast<item **>(buf) = this;
	else
		throw_(error_value_type::cannot_handle_request);
}

void winp::thread::item::do_apply_(const void *value, const std::type_info &id){}

HWND winp::thread::item::get_handle_() const{
	return handle_;
}

void winp::thread::item::throw_(error_value_type value) const{
	app::object::error = value;
}
