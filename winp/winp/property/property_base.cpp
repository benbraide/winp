#include "../app/app_object.h"

winp::prop::base::base() = default;

winp::prop::base::~base() = default;

void winp::prop::base::init_(change_callback_type changed_callback){
	changed_callback_ = changed_callback;
}

void winp::prop::base::init_(change_callback_type callback, setter_type setter, getter_type getter){
	init_(callback);
}

void winp::prop::base::change_(const void *value, std::size_t size){}

bool winp::prop::base::changed_(const void *value_ref, std::size_t context) const{
	return ((changed_callback_ == nullptr) ? true : changed_callback_(*this, value_ref, context));
}

void winp::prop::base::get_value_(void *buf, std::size_t context) const{}

void winp::prop::base::throw_(error_value_type value) const{
	app::object::error = value;
}
