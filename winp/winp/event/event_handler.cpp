#include "../app/app_object.h"

void winp::event::unhandled_handler::handle_unhandled_event_(object &e){}

void winp::event::create_destroy_handler::handle_create_event_(object &e){}

void winp::event::create_destroy_handler::handle_destroy_event_(object &e){}

void winp::event::draw_handler::handle_background_erase_event_(draw &e){
	auto visible_surface = dynamic_cast<ui::visible_surface *>(this);
	if (visible_surface != nullptr){
		auto drawer = dynamic_cast<draw &>(e).get_drawer_();
		if (drawer != nullptr)
			drawer->Clear(visible_surface->get_background_color_());
	}
}

void winp::event::draw_handler::handle_paint_event_(draw &e){}

void winp::event::mouse_handler::handle_mouse_leave_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_enter_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_move_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_wheel_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_down_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_up_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_double_click_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_drag_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_drag_begin_event_(mouse &e){}

void winp::event::mouse_handler::handle_mouse_drag_end_event_(mouse &e){}

void winp::event::focus_handler::handle_set_focus_event_(object &e){}

void winp::event::focus_handler::handle_kill_focus_event_(object &e){}

bool winp::event::key_handler::handle_key_down_event_(key &e){
	return true;
}

bool winp::event::key_handler::handle_key_up_event_(key &e){
	return true;
}

bool winp::event::key_handler::handle_key_char_event_(key &e){
	return true;
}
