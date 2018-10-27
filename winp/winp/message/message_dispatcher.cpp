#include "../app/app_object.h"
#include "message_dispatcher.h"

void winp::message::dispatcher::cleanup_(){}

void winp::message::dispatcher::dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default, bool is_post){
	auto pre_response = pre_dispatch_(target, msg, wparam, lparam, result, call_default);
	if (pre_response != event_result_type::prevent_default){
		auto saved_result = result;
		do_dispatch_(target, msg, wparam, lparam, result, call_default);
		post_dispatch_(target, msg, wparam, lparam, result, call_default);

		if (pre_response == event_result_type::result_set)
			result = saved_result;//Use saved result
	}

	if (!is_post)
		cleanup_();
}

winp::message::dispatcher::event_result_type winp::message::dispatcher::pre_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool &call_default){
	return event_result_type::nil;
}

void winp::message::dispatcher::post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){}

void winp::message::dispatcher::do_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){
	auto event_response = fire_event_(target, msg, wparam, lparam, result, call_default);
	if (event_response != event_result_type::prevent_default)
		do_default_(target, msg, wparam, lparam, result, call_default, (event_response == event_result_type::result_set));
}

void winp::message::dispatcher::do_default_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default, bool result_set){
	message::basic info(target, message::basic::info_type{
		msg,
		wparam, 
		lparam
	});

	auto object_response = target.handle_message_(info);
	if (object_response == event_result_type::prevent_default)
		return;//Skip further handling

	if (object_response == event_result_type::result_set && !result_set)
		result = info.get_result();//Update result

	if (call_default){//Do system handling
		auto entry = get_default_message_entry_of_(target);
		if (entry != nullptr && object_response != event_result_type::result_set && !result_set)
			result = CallWindowProcW(entry, get_handle_of_(target), msg, wparam, lparam);
		else if (entry != nullptr)//Result already set
			CallWindowProcW(entry, get_handle_of_(target), msg, wparam, lparam);
	}
}

winp::message::dispatcher::event_result_type winp::message::dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){
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

bool winp::message::dispatcher::bubble_of_(event::object &e){
	return e.bubble_();
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

winp::message::dispatcher::event_result_type winp::message::create_destroy_dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return event_result_type::nil;

	auto e = create_event_<event::message>(&target, msg, wparam, lparam, &result, call_default);
	if (msg == WM_CREATE)
		fire_event_of_(*window_target, window_target->create_event, *e);
	else
		fire_event_of_(*window_target, window_target->destroy_event, *e);

	return (default_prevented_of_(*e) ? event_result_type::prevent_default : (result_set_of_(*e) ? event_result_type::result_set : event_result_type::nil));
}

void winp::message::draw_dispatcher::cleanup_(){
	e_ = nullptr;
}

void winp::message::draw_dispatcher::post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){
	ui::surface *child_target;
	auto saved_offset = offset_;

	for (auto child : get_children_of_(target)){
		if ((child_target = dynamic_cast<ui::surface *>(child)) != nullptr && dynamic_cast<ui::window_surface *>(child) == nullptr)
			dispatch_(*child_target, msg, wparam, lparam, result, false, true);
		offset_ = saved_offset;//Restore saved offset
	}
}

winp::message::dispatcher::event_result_type winp::message::draw_dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){
	auto visible_target = dynamic_cast<ui::visible_surface *>(&target);
	if (visible_target == nullptr)
		return event_result_type::nil;

	if (e_ == nullptr){//Initialize
		e_ = create_event_<event::draw>(&target, msg, wparam, lparam, &result, call_default);
		offset_ = POINT{};
	}

	e_->set_target_(visible_target, offset_);
	fire_event_of_(*visible_target, visible_target->draw_event, *e_);

	if (msg == WM_ERASEBKGND && !default_prevented_of_(*e_)){
		auto drawer = e_->get_drawer_();
		if (drawer != nullptr)
			drawer->Clear(visible_target->get_background_color_());
	}

	return (default_prevented_of_(*e_) ? event_result_type::prevent_default : (result_set_of_(*e_) ? event_result_type::result_set : event_result_type::nil));
}

void winp::message::mouse_dispatcher::cleanup_(){
	e_ = nullptr;
	ev_ = nullptr;
}

void winp::message::mouse_dispatcher::post_dispatch_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){
	if (e_ == nullptr){//Target is not a 'ui::io_surface'
		auto parent = target.get_first_ancestor_of_<ui::io_surface>();
		if (parent != nullptr)
			dispatch_(*parent, msg, wparam, lparam, result, call_default, true);
	}
	else if (e_->bubble_to_type_<ui::io_surface>())
		dispatch_(*dynamic_cast<ui::surface *>(e_->get_context()), msg, wparam, lparam, result, false, true);
}

winp::message::dispatcher::event_result_type winp::message::mouse_dispatcher::fire_event_(ui::surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result, bool call_default){
	auto io_target = dynamic_cast<ui::io_surface *>(&target);
	if (io_target == nullptr)
		return event_result_type::nil;

	if (e_ == nullptr){//Initialize
		auto mouse_position = GetMessagePos();
		POINT computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };

		resolve_(*io_target, msg, wparam, lparam, ev_, button_);
		e_ = create_event_<event::mouse>(&target, msg, wparam, lparam, &result, call_default, computed_mouse_position, button_);
	}

	if (ev_ != nullptr && e_ != nullptr)
		fire_event_of_(*io_target, *ev_, *e_);

	return (default_prevented_of_(*e_) ? event_result_type::prevent_default : (result_set_of_(*e_) ? event_result_type::result_set : event_result_type::nil));
}

void winp::message::mouse_dispatcher::resolve_(ui::io_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, event::manager_base *&ev, event::mouse::button_type &button){
	switch (msg){
	case WINP_WM_MOUSELEAVE:
		ev = &target.mouse_event.leave;
		button = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEENTER:
		ev = &target.mouse_event.enter;
		button = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEMOVE:
		ev = &target.mouse_event.move;
		button = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEWHEEL:
		ev = &target.mouse_event.wheel;
		button = event::mouse::button_type::nil;
		break;
	case WM_LBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
		ev = &target.mouse_event.down;
		button = event::mouse::button_type::left;
		break;
	case WM_MBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
		ev = &target.mouse_event.down;
		button = event::mouse::button_type::middle;
		break;
	case WM_RBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
		ev = &target.mouse_event.down;
		button = event::mouse::button_type::right;
		break;
	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP:
		ev = &target.mouse_event.up;
		button = event::mouse::button_type::left;
		break;
	case WM_MBUTTONUP:
	case WM_NCMBUTTONUP:
		ev = &target.mouse_event.up;
		button = event::mouse::button_type::middle;
		break;
	case WM_RBUTTONUP:
	case WM_NCRBUTTONUP:
		ev = &target.mouse_event.up;
		button = event::mouse::button_type::right;
		break;
	case WM_LBUTTONDBLCLK:
	case WM_NCLBUTTONDBLCLK:
		ev = &target.mouse_event.double_click;
		button = event::mouse::button_type::left;
		break;
	case WM_MBUTTONDBLCLK:
	case WM_NCMBUTTONDBLCLK:
		ev = &target.mouse_event.double_click;
		button = event::mouse::button_type::middle;
		break;
	case WM_RBUTTONDBLCLK:
	case WM_NCRBUTTONDBLCLK:
		ev = &target.mouse_event.double_click;
		button = event::mouse::button_type::right;
		break;
	case WINP_WM_MOUSEDRAG:
		ev = &target.mouse_event.drag;
		button = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEDRAGBEGIN:
		ev = &target.mouse_event.drag_begin;
		button = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEDRAGEND:
		ev = &target.mouse_event.drag_end;
		button = event::mouse::button_type::nil;
		break;
	default:
		ev = nullptr;
		button = event::mouse::button_type::nil;
		break;
	}
}
