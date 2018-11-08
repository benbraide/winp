#include "../app/app_object.h"

void winp::event::dispatcher::dispatch_(object &e){
	e.get_context()->handle_event_(e);
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
	auto handler = dynamic_cast<ui::visible_surface *>(e.get_context());
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
