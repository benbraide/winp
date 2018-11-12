#include "../app/app_object.h"

void winp::event::dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<unhandled_handler *>(e.get_context());
	if (handler != nullptr)
		handler->handle_unhandled_event_(e);
}

void winp::event::dispatcher::set_result_of_(event::object &e, LRESULT value, bool always_set){
	message::dispatcher::set_result_of_(e, value, always_set);
}

LRESULT winp::event::dispatcher::get_result_of_(event::object &e){
	return message::dispatcher::get_result_of_(e);
}

bool winp::event::dispatcher::result_set_of_(event::object &e){
	return message::dispatcher::result_set_of_(e);
}

bool winp::event::dispatcher::default_prevented_of_(event::object &e){
	return message::dispatcher::default_prevented_of_(e);
}

void winp::event::create_destroy_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<create_destroy_handler *>(e.get_context());
	if (handler != nullptr){
		if (e.get_info()->code == WM_CREATE)
			handler->handle_create_event_(e);
		else//Destroy event
			handler->handle_destroy_event_(e);
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::draw_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<draw_handler *>(e.get_context());
	if (handler != nullptr){
		switch (e.get_info()->code){
		case WM_ERASEBKGND:
			handler->handle_background_erase_event_(dynamic_cast<draw &>(e));
			break;
		case WM_PAINT:
		case WM_PRINTCLIENT:
			handler->handle_paint_event_(dynamic_cast<draw &>(e));
			break;
		default:
			break;
		}
	}
	else if (e.get_info()->code == WM_ERASEBKGND && dynamic_cast<unhandled_handler *>(e.get_context()) == nullptr)//Do default painting
		erase_background_(dynamic_cast<draw &>(e));
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::draw_dispatcher::erase_background_(draw &e){
	auto visible_surface = dynamic_cast<ui::visible_surface *>(e.get_context());
	if (visible_surface != nullptr && !visible_surface->is_transparent_()){
		auto drawer = e.get_drawer_();
		if (drawer != nullptr)
			drawer->Clear(visible_surface->get_background_color_());
	}
}

void winp::event::cursor_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<cursor_handler *>(e.get_context());
	if (handler != nullptr)
		handler->handle_set_cursor_event_(dynamic_cast<cursor &>(e));
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::mouse_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<mouse_handler *>(e.get_context());
	if (handler != nullptr){
		switch (e.get_info()->code){
		case WINP_WM_MOUSELEAVE:
			handler->handle_mouse_leave_event_(dynamic_cast<mouse &>(e));
			break;
		case WINP_WM_MOUSEENTER:
			handler->handle_mouse_enter_event_(dynamic_cast<mouse &>(e));
			break;
		case WINP_WM_MOUSEMOVE:
			handler->handle_mouse_move_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			handler->handle_mouse_wheel_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			handler->handle_mouse_down_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			handler->handle_mouse_up_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			handler->handle_mouse_double_click_event_(dynamic_cast<mouse &>(e));
			break;
		case WINP_WM_MOUSEDRAG:
			handler->handle_mouse_drag_event_(dynamic_cast<mouse &>(e));
			break;
		case WINP_WM_MOUSEDRAGBEGIN:
			handler->handle_mouse_drag_begin_event_(dynamic_cast<mouse &>(e));
			break;
		case WINP_WM_MOUSEDRAGEND:
			handler->handle_mouse_drag_end_event_(dynamic_cast<mouse &>(e));
			break;
		default:
			break;
		}
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::focus_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<focus_handler *>(e.get_context());
	if (handler != nullptr){
		if (e.get_info()->code == WM_SETFOCUS)
			handler->handle_set_focus_event_(e);
		else//Destroy event
			handler->handle_kill_focus_event_(e);
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::key_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<key_handler *>(e.get_context());
	if (handler != nullptr){
		switch (e.get_info()->code){
		case WM_KEYDOWN:
			if (handler->handle_key_down_event_(dynamic_cast<key &>(e)))
				e.stop_propagation();
			break;
		case WM_KEYUP:
			if (handler->handle_key_up_event_(dynamic_cast<key &>(e)))
				e.stop_propagation();
			break;
		case WM_CHAR:
			if (handler->handle_key_char_event_(dynamic_cast<key &>(e)))
				e.stop_propagation();
			break;
		default:
			break;
		}
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}
