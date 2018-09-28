#include "../app/app_object.h"

winp::message::object::object(ui::object *target)
	: owner_(target), target_(target){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &owner)
			*static_cast<ui::object **>(buf) = owner_;
		else if (&prop == &this->target)
			*static_cast<ui::object **>(buf) = target_;
	};

	owner.init_(nullptr, nullptr, getter);
	this->target.init_(nullptr, nullptr, getter);
}

winp::message::object::~object() = default;

winp::message::basic::basic(ui::object *target, const info_type &info)
	: object(target), info_(info), result_(0){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t indcontextex){
		if (&prop == &result)
			result_ = *static_cast<const LRESULT *>(value);
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &this->info)
			*static_cast<info_type **>(buf) = &info_;
		else if (&prop == &result)
			*static_cast<LRESULT *>(buf) = result_;
	};

	this->info.init_(nullptr, nullptr, getter);
	result.init_(nullptr, setter, getter);
}

winp::message::basic::~basic() = default;
