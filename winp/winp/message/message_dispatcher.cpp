#include "message_dispatcher.h"

bool winp::message::dispatcher::dispatch_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	message::basic info(&target, message::basic::info_type{
		msg,
		wparam, 
		lparam
	});

	auto prevent_default = target.handle_message_(info);
	result = info.result;

	return prevent_default;
}

bool winp::message::dispatcher::fire_event_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	return false;
}

bool winp::message::create_destroy_dispatcher::dispatch_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	return fire_event_(target, msg, wparam, lparam, result);
}

bool winp::message::create_destroy_dispatcher::fire_event_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	event::object e(&target);
	if (msg == WM_CREATE)
		target.fire_event_(target.create_event_, e);
	else
		target.fire_event_(target.destroy_event_, e);

	result = 0;
	return e.prevent_default;
}
