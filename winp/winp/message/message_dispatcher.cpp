#include "../app/app_object.h"
#include "message_dispatcher.h"

void winp::message::dispatcher::dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const{
	do_dispatch_(target, msg, wparam, lparam, result, call_default);
	post_dispatch_(target, msg, wparam, lparam, result);
}

void winp::message::dispatcher::do_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const{
	auto event_response = fire_event_(target, msg, wparam, lparam, result);
	if (event_response == event_result_type::prevent_default)
		return;//Skip default handling

	message::basic info(target, message::basic::info_type{
		msg,
		wparam, 
		lparam
	});

	auto object_response = target.handle_message_(info);
	if (object_response == event_result_type::prevent_default)
		return;//Skip further handling

	if (object_response == event_result_type::result_set && event_response != event_result_type::result_set)
		result = info.get_result();//Update result

	auto entry = get_default_message_entry_of_(target);
	if (entry != nullptr && object_response != event_result_type::result_set && event_response != event_result_type::result_set)
		result = CallWindowProcW(entry, get_handle_of_(target), msg, wparam, lparam);
	else if (entry != nullptr)//Result already set
		CallWindowProcW(entry, get_handle_of_(target), msg, wparam, lparam);
}

winp::message::dispatcher::event_result_type winp::message::dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	return event_result_type::nil;
}

winp::message::dispatcher::event_result_type winp::message::dispatcher::post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	return event_result_type::nil;
}

std::size_t winp::message::dispatcher::event_handlers_count_of_(ui::surface &target, event::manager_base &ev){
	return target.event_handlers_count_(ev);
}

void winp::message::dispatcher::fire_event_of_(ui::surface &target, event::manager_base &ev, event::object &e){
	target.fire_event_(ev, e);
}

HWND winp::message::dispatcher::get_handle_of_(ui::surface &target){
	return target.get_handle_();
}

WNDPROC winp::message::dispatcher::get_default_message_entry_of_(ui::surface &target){
	return target.get_default_message_entry_();
}

std::list<winp::ui::object *> &winp::message::dispatcher::get_children_of_(ui::tree &target){
	return target.children_;
}

void winp::message::dispatcher::set_flag_of_(event::object &e, unsigned int flag){
	e.state_ |= flag;
}

void winp::message::dispatcher::remove_flag_of_(event::object &e, unsigned int flag){
	e.state_ &= ~flag;
}

bool winp::message::dispatcher::default_prevented_of_(event::object &e){
	return e.default_prevented_();
}

bool winp::message::dispatcher::propagation_stopped_of_(event::object &e){
	return e.propagation_stopped_();
}

bool winp::message::dispatcher::result_set_of_(event::object &e){
	return e.result_set_();
}

winp::message::dispatcher::event_result_type winp::message::create_destroy_dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return event_result_type::nil;

	event::object e(target);
	if (msg == WM_CREATE)
		fire_event_of_(*window_target, window_target->create_event, e);
	else
		fire_event_of_(*window_target, window_target->destroy_event, e);

	return event_result_type::nil;
}

winp::message::dispatcher::event_result_type winp::message::draw_dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	auto visible_target = dynamic_cast<ui::visible_surface *>(&target);
	if (visible_target == nullptr)
		return event_result_type::nil;

	if (e_ == nullptr){//Initialize
		e_ = std::make_shared<event::draw>(target, event::message::info_type{ msg, wparam, lparam });
		offset_ = POINT{};
	}

	e_->set_target_(visible_target, offset_);
	fire_event_of_(*visible_target, visible_target->draw_event, *e_);

	if (msg == WM_ERASEBKGND && !default_prevented_of_(*e_)){
		auto drawer = e_->get_drawer_();
		if (drawer != nullptr)
			drawer->Clear(visible_target->get_background_color_());
	}

	return (default_prevented_of_(*e_) ? event_result_type::prevent_default : event_result_type::nil);
}

winp::message::dispatcher::event_result_type winp::message::draw_dispatcher::post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	ui::surface *child_target;
	auto saved_offset = offset_;

	for (auto child : get_children_of_(target)){
		if ((child_target = dynamic_cast<ui::surface *>(child)) != nullptr && dynamic_cast<ui::window_surface *>(child) == nullptr)
			dispatch_(*child_target, msg, wparam, lparam, result, false);
		offset_ = saved_offset;//Restore saved offset
	}

	e_ = nullptr;//Reset event
	return event_result_type::nil;
}
