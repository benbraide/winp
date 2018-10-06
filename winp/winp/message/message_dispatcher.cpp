#include "../app/app_object.h"
#include "message_dispatcher.h"

void winp::message::dispatcher::dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const{
	message::basic info(&target, message::basic::info_type{
		msg,
		wparam, 
		lparam
	});

	if (!target.handle_message_(info) && call_default){
		auto window_target = dynamic_cast<ui::window_surface *>(&target);
		if (window_target != nullptr)
			result = CallWindowProcW(get_default_message_entry_of_(*window_target), window_target->get_handle_(), msg, wparam, lparam);
		else
			result = info.result;
	}
	else
		result = info.result;
}

winp::message::dispatcher::event_result_type winp::message::dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	return event_result_type::nil;
}

void winp::message::dispatcher::fire_event_of_(ui::surface &target, event::manager_base &ev, event::object &e){
	target.fire_event_(ev, e);
}

HWND winp::message::dispatcher::get_handle_of_(ui::surface &target){
	return target.get_handle_();
}

WNDPROC winp::message::dispatcher::get_default_message_entry_of_(ui::window_surface &target){
	return target.get_default_message_entry_();
}

void winp::message::create_destroy_dispatcher::dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const{
	fire_event_(target, msg, wparam, lparam, result);
	if (call_default){
		auto window_target = dynamic_cast<ui::window_surface *>(&target);
		if (window_target != nullptr)
			result = CallWindowProcW(get_default_message_entry_of_(*window_target), get_handle_of_(target), msg, wparam, lparam);
		else
			result = 0;
	}
	else
		result = 0;
}

winp::message::dispatcher::event_result_type winp::message::create_destroy_dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return event_result_type::nil;

	event::object e(&target);
	if (msg == WM_CREATE)
		fire_event_of_(*window_target, window_target->create_event, e);
	else
		fire_event_of_(*window_target, window_target->destroy_event, e);

	result = 0;
	return event_result_type::nil;
}
