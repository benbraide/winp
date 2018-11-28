#include "../app/app_object.h"

void winp::event::unhandled_handler::handle_unhandled_event_(object &e){
	if (e.get_info()->message == WM_ERASEBKGND)
		draw_dispatcher::erase_background_(dynamic_cast<draw &>(e));
}

bool winp::event::tree_handler::handle_parent_change_event_(tree &e){
	return true;
}

bool winp::event::tree_handler::handle_index_change_event_(tree &e){
	return true;
}

bool winp::event::tree_handler::handle_child_index_change_event_(tree &e){
	return true;
}

bool winp::event::tree_handler::handle_child_insert_event_(tree &e){
	return true;
}

bool winp::event::tree_handler::handle_child_remove_event_(tree &e){
	return true;
}

void winp::event::tree_handler::handle_parent_changed_event_(tree &e){}

void winp::event::tree_handler::handle_index_changed_event_(tree &e){}

void winp::event::tree_handler::handle_child_index_changed_event_(tree &e){}

void winp::event::tree_handler::handle_child_inserted_event_(tree &e){}

void winp::event::tree_handler::handle_child_removed_event_(tree &e){}

void winp::event::create_destroy_handler::handle_create_event_(object &e){}

void winp::event::create_destroy_handler::handle_destroy_event_(object &e){}

void winp::event::draw_handler::handle_background_erase_event_(draw &e){
	draw_dispatcher::erase_background_(e);
}

void winp::event::draw_handler::handle_paint_event_(draw &e){}

void winp::event::draw_item_handler::handle_draw_item_event_(draw_item &e){}

void winp::event::draw_item_handler::handle_measure_item_event_(measure_item &e){}

void winp::event::cursor_handler::handle_set_cursor_event_(cursor &e){}

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

void winp::event::menu_handler::handle_menu_uninit_event_(object &e){}

void winp::event::menu_handler::handle_menu_init_event_(object &e){}

void winp::event::menu_handler::handle_menu_init_item_event_(object &e){}

void winp::event::menu_handler::handle_menu_select_event_(object &e){}

void winp::event::menu_handler::handle_menu_check_event_(object &e){}

void winp::event::menu_handler::handle_menu_uncheck_event_(object &e){}

bool winp::event::menu_handler::handle_context_menu_query_event_(context_menu_prefix &e){
	return false;
}

winp::menu::object *winp::event::menu_handler::handle_context_menu_request_event_(context_menu_prefix &e){
	return nullptr;
}

void winp::event::menu_handler::handle_context_menu_event_(context_menu &e){}

bool winp::event::frame_handler::handle_close_event_(object &e){
	return true;
}

bool winp::event::frame_handler::handle_position_change_event_(position &e){
	return true;
}

bool winp::event::frame_handler::handle_size_change_event_(size &e){
	return true;
}

void winp::event::frame_handler::handle_maximized_event_(object &e){}

void winp::event::frame_handler::handle_minimized_event_(object &e){}

void winp::event::frame_handler::handle_position_changed_event_(object &e){}

void winp::event::frame_handler::handle_size_changed_event_(object &e){}
