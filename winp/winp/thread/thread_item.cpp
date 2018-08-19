#include "thread_object.h"

winp::thread::item::item(){
	auto setter = [this](const prop::base<item> &prop, const void *value, std::size_t index){
		if (&prop == &owner)
			change_owner_(*static_cast<object **>(const_cast<void *>(value)));
	};

	auto getter = [this](const prop::base<item> &prop, void *buf, std::size_t index){
		if (&prop == &owner)
			*static_cast<object **>(buf) = owner_;
		else if (&prop == &handle)
			*static_cast<HWND *>(buf) = handle_;
	};

	owner.init_(*this, nullptr, setter, getter, &error);
	handle.init_(*this, nullptr, nullptr, getter, &error);
}

void winp::thread::item::change_owner_(object *value){
	owner_ = value;
}

winp::thread::item::~item() = default;
