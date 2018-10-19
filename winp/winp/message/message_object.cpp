#include "../app/app_object.h"

winp::message::object::object(ui::object &target)
	: target_(&target){}

winp::message::object::~object() = default;

winp::ui::object *winp::message::object::get_target() const{
	return (is_thread_context_() ? target_ : nullptr);
}

bool winp::message::object::is_thread_context_() const{
	return target_->get_thread().is_thread_context();
}

winp::message::basic::basic(ui::object &target, const info_type &info)
	: object(target), info_(info), result_(0){}

winp::message::basic::info_type winp::message::basic::get_info() const{
	return (is_thread_context_() ? info_ : info_type{});
}

void winp::message::basic::set_result(bool value){
	set_result(value ? TRUE : FALSE);
}

LRESULT winp::message::basic::get_result() const{
	return (is_thread_context_() ? result_ : LRESULT());
}

winp::message::basic::~basic() = default;
