#include "../app/app_object.h"
#include "message_dispatcher.h"

void winp::message::dispatcher::dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const{
	message::basic info(target, message::basic::info_type{
		msg,
		wparam, 
		lparam
	});

	if (!target.handle_message_(info) && call_default){
		auto window_target = dynamic_cast<ui::window_surface *>(&target);
		if (window_target != nullptr)
			result = CallWindowProcW(get_default_message_entry_of_(*window_target), window_target->get_handle_(), msg, wparam, lparam);
		else
			result = info.get_result();
	}
	else
		result = info.get_result();
}

winp::message::dispatcher::event_result_type winp::message::dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
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

WNDPROC winp::message::dispatcher::get_default_message_entry_of_(ui::window_surface &target){
	return target.get_default_message_entry_();
}

std::list<winp::ui::object *> &winp::message::dispatcher::get_children_of_(ui::tree &target){
	return target.children_;
}

bool winp::message::dispatcher::default_prevented_of(event::object &e){
	return e.default_prevented_();
}

bool winp::message::dispatcher::propagation_stopped_of(event::object &e){
	return e.propagation_stopped_();
}

bool winp::message::dispatcher::result_set_of(event::object &e){
	return e.result_set_();
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

	event::object e(target);
	if (msg == WM_CREATE)
		fire_event_of_(*window_target, window_target->create_event, e);
	else
		fire_event_of_(*window_target, window_target->destroy_event, e);

	result = 0;
	return event_result_type::nil;
}

void winp::message::draw_dispatcher::dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default) const{
	if (fire_event_(target, msg, wparam, lparam, result) != event_result_type::prevent_default && call_default){
		auto window_target = dynamic_cast<ui::window_surface *>(&target);
		if (window_target != nullptr)
			result = CallWindowProcW(get_default_message_entry_of_(*window_target), get_handle_of_(target), msg, wparam, lparam);
		else
			result = 0;
	}
	else
		result = 0;
}

winp::message::dispatcher::event_result_type winp::message::draw_dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const{
	auto visible_target = dynamic_cast<ui::visible_surface *>(&target);
	if (visible_target == nullptr)
		return event_result_type::nil;

	if (msg != WM_ERASEBKGND && get_children_of_(target).empty() && event_handlers_count_of_(target, visible_target->draw_event) == 0u)
		return event_result_type::nil;//No handlers, no offspring -- do default

	event::draw e(target, event::message::info_type{ msg, wparam, lparam });
	return fire_event_(e, dynamic_cast<ui::visible_surface *>(&target), msg, wparam, lparam, result, POINT{});
}

winp::message::dispatcher::event_result_type winp::message::draw_dispatcher::fire_event_(event::draw &e, ui::tree *target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, POINT offset) const{
	if (target == nullptr)
		return event_result_type::nil;

	auto visible_target = dynamic_cast<ui::visible_surface *>(target);
	if (visible_target != nullptr){
		e.set_target_(visible_target, offset);
		fire_event_of_(*visible_target, visible_target->draw_event, e);

		if (msg == WM_ERASEBKGND && !default_prevented_of(e)){
			auto drawer = e.get_drawer_();
			if (drawer != nullptr)
				drawer->Clear(visible_target->get_background_color_());
		}
	}

	for (auto child : get_children_of_(*target)){
		if ((target = dynamic_cast<ui::tree *>(child)) != nullptr && dynamic_cast<ui::window_surface *>(child) == nullptr)
			fire_event_(e, target, msg, wparam, lparam, result, offset);
	}

	return (default_prevented_of(e) ? event_result_type::prevent_default : event_result_type::nil);
}
