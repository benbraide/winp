#include "thread_object.h"

winp::thread::item::item()
	: owner_(nullptr), handle_(nullptr){}

winp::thread::item::item(object &owner)
	: owner_(&owner), handle_(nullptr){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t index){
		if (&prop == &this->owner)
			*static_cast<object **>(buf) = owner_;
		else if (&prop == &handle)
			*static_cast<HWND *>(buf) = handle_;
	};

	this->owner.init_(nullptr, nullptr, getter);
	handle.init_(nullptr, nullptr, getter);
}

winp::thread::item::~item() = default;

void winp::thread::item::destroy_(){}
