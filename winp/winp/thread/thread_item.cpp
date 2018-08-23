#include "thread_object.h"

winp::thread::item::item()
	: owner_(nullptr), handle_(nullptr){}

winp::thread::item::item(object &owner)
	: owner_(&owner), handle_(nullptr){
	auto getter = [this](const prop::base<item> &prop, void *buf, std::size_t index){
		if (&prop == &this->owner)
			*static_cast<object **>(buf) = owner_;
		else if (&prop == &handle)
			*static_cast<HWND *>(buf) = handle_;
	};

	this->owner.init_(*this, nullptr, nullptr, getter);
	handle.init_(*this, nullptr, nullptr, getter);
}

winp::thread::item::~item() = default;
