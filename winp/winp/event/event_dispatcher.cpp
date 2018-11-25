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

void winp::event::tree_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<tree_handler *>(e.get_context());
	if (handler != nullptr){
		switch (e.get_info()->message){
		case WINP_WM_PARENT_CHANGED:
			handler->handle_parent_change_event_(dynamic_cast<tree &>(e));
			break;
		case WINP_WM_INDEX_CHANGED:
			handler->handle_index_change_event_(dynamic_cast<tree &>(e));
			break;
		case WINP_WM_CHILD_INDEX_CHANGED:
			handler->handle_child_index_change_event_(dynamic_cast<tree &>(e));
			break;
		case WINP_WM_CHILD_INSERTED:
			handler->handle_child_insert_event_(dynamic_cast<tree &>(e));
			break;
		case WINP_WM_CHILD_REMOVED:
			handler->handle_child_remove_event_(dynamic_cast<tree &>(e));
			break;
		default:
			break;
		}
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::create_destroy_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<create_destroy_handler *>(e.get_context());
	if (handler != nullptr){
		if (e.get_info()->message == WM_CREATE)
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
		switch (e.get_info()->message){
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
	else if (e.get_info()->message == WM_ERASEBKGND && dynamic_cast<unhandled_handler *>(e.get_context()) == nullptr)//Do default painting
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

void winp::event::draw_item_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<draw_item_handler *>(e.get_context());
	if (handler != nullptr){
		switch (e.get_info()->message){
		case WM_MEASUREITEM:
			handler->handle_measure_item_event_(dynamic_cast<measure_item &>(e));
			break;
		case WM_DRAWITEM:
			handler->handle_draw_item_event_(dynamic_cast<draw_item &>(e));
			break;
		default:
			break;
		}
	}
	else if (e.get_info()->message == WM_DRAWITEM && dynamic_cast<unhandled_handler *>(e.get_context()) == nullptr)//Do default handler
		draw_item_(dynamic_cast<draw_item &>(e));
	else if (e.get_info()->message == WM_MEASUREITEM && dynamic_cast<unhandled_handler *>(e.get_context()) == nullptr)//Do default handler
		measure_item_(dynamic_cast<measure_item &>(e));
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::draw_item_dispatcher::draw_item_(draw_item &e){
	auto menu_item = dynamic_cast<menu::item_component *>(e.get_context());
	if (menu_item != nullptr)//Draw menu item
		draw_menu_item_(*menu_item, e);
}

void winp::event::draw_item_dispatcher::measure_item_(measure_item &e){
	auto menu_item = dynamic_cast<menu::item_component *>(e.get_context());
	if (menu_item != nullptr)//Measure menu item
		measure_menu_item_(*menu_item, e);
}

void winp::event::draw_item_dispatcher::draw_menu_item_(menu::item_component &item, draw_item &e){
	if (dynamic_cast<menu::separator *>(&item) == nullptr){
		DWORD text_color = 0u;
		HBRUSH backgroud_brush = nullptr;

		if (item.is_popup_item_()){
			if ((e.struct_.itemState & ODS_DISABLED) == 0u){
				if ((e.struct_.itemState & ODS_SELECTED) == 0u){
					text_color = GetSysColor(COLOR_MENUTEXT);
					backgroud_brush = GetSysColorBrush(COLOR_MENU);
				}
				else{//Selected item
					text_color = GetSysColor(COLOR_HIGHLIGHTTEXT);
					backgroud_brush = GetSysColorBrush(COLOR_MENUHILIGHT);
				} 
			}
			else{//Disabled item
				text_color = GetSysColor(COLOR_GRAYTEXT);
				backgroud_brush = GetSysColorBrush(COLOR_MENU);
			}
		}
		else if ((e.struct_.itemState & ODS_DISABLED) == 0u){//Enabled bar item
			if ((e.struct_.itemState & ODS_SELECTED) == 0u){
				text_color = GetSysColor(COLOR_MENUTEXT);
				backgroud_brush = GetSysColorBrush(COLOR_MENUBAR);
			}
			else{//Selected item
				text_color = GetSysColor(COLOR_HIGHLIGHTTEXT);
				backgroud_brush = GetSysColorBrush(COLOR_MENUHILIGHT);
			}
		}
		else{//Disabled bar item
			text_color = GetSysColor(COLOR_GRAYTEXT);
			backgroud_brush = GetSysColorBrush(COLOR_MENUBAR);
		}

		if (backgroud_brush != nullptr)
			FillRect(e.struct_.hDC, &e.struct_.rcItem, backgroud_brush);

		auto label = item.get_label_();
		if (label != nullptr && !label->empty()){
			HFONT bold_font = nullptr, font = item.font_;
			auto non_separator_item = dynamic_cast<menu::item *>(&item);

			if (non_separator_item != nullptr && non_separator_item->is_default()){
				LOGFONTW font_info{};
				GetObjectW(((font == nullptr) ? GetStockFont(SYSTEM_FONT) : font), static_cast<int>(sizeof(LOGFONTW)), &font_info);
				if (font_info.lfWeight < FW_BOLD){//Create bold font
					font_info.lfWeight = FW_BOLD;
					bold_font = CreateFontIndirectW(&font_info);
				}
			}

			auto old_font = SelectObject(e.struct_.hDC, ((bold_font == nullptr) ? font : bold_font));
			auto old_text_color = SetTextColor(e.struct_.hDC, text_color);
			auto old_background_mode = SetBkMode(e.struct_.hDC, TRANSPARENT);

			TextOutW(e.struct_.hDC, (e.struct_.rcItem.left + GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE)), e.struct_.rcItem.top, label->data(), static_cast<int>(label->size()));
			SetBkMode(e.struct_.hDC, old_background_mode);//Restore background mode
			SetTextColor(e.struct_.hDC, old_text_color);//Restore text color
			SelectObject(e.struct_.hDC, old_font);//Restore font

			if (bold_font != nullptr)//Destroy created font
				DeleteObject(bold_font);
		}
	}
	else{//Separator
		auto theme = OpenThemeData(e.get_info()->hwnd, L"MENU");
		DrawThemeBackground(theme, e.struct_.hDC, MENU_POPUPSEPARATOR, 0, &e.struct_.rcItem, nullptr);
		CloseThemeData(theme);
	}

	e.stop_propagation();
}

void winp::event::draw_item_dispatcher::measure_menu_item_(menu::item_component &item, measure_item &e){
	auto device = GetDC(GetDesktopWindow());
	if (device == nullptr)//Failed to retrieve device
		return;

	auto label = item.get_label_();
	e.set_size(control::object::compute_size(GetDesktopWindow(), device, item.font_, ((label == nullptr) ? L"" : *label)));

	ReleaseDC(GetDesktopWindow(), device);
	e.stop_propagation();
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
		switch (e.get_info()->message){
		case WM_NCMOUSELEAVE:
		case WM_MOUSELEAVE:
		case WINP_WM_MOUSELEAVE:
			handler->handle_mouse_leave_event_(dynamic_cast<mouse &>(e));
			break;
		case WINP_WM_MOUSEENTER:
			handler->handle_mouse_enter_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_NCMOUSEMOVE:
		case WM_MOUSEMOVE:
		case WINP_WM_MOUSEMOVE:
			handler->handle_mouse_move_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			handler->handle_mouse_wheel_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			handler->handle_mouse_down_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_NCLBUTTONUP:
		case WM_NCMBUTTONUP:
		case WM_NCRBUTTONUP:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			handler->handle_mouse_up_event_(dynamic_cast<mouse &>(e));
			break;
		case WM_NCLBUTTONDBLCLK:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCRBUTTONDBLCLK:
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
		if (e.get_info()->message == WM_SETFOCUS)
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
		switch (e.get_info()->message){
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

void winp::event::menu_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<menu_handler *>(e.get_context());
	if (handler != nullptr){
		switch (e.get_info()->message){
		case WM_UNINITMENUPOPUP:
			handler->handle_menu_uninit_event_(e);
			break;
		case WM_INITMENUPOPUP:
			handler->handle_menu_init_event_(e);
			break;
		case WINP_WM_MENU_INIT_ITEM:
			handler->handle_menu_init_item_event_(e);
			break;
		case WINP_WM_MENU_SELECT:
			handler->handle_menu_select_event_(e);
			break;
		case WINP_WM_MENU_CHECK:
			handler->handle_menu_check_event_(e);
			break;
		case WINP_WM_MENU_UNCHECK:
			handler->handle_menu_uncheck_event_(e);
			break;
		case WINP_WM_CONTEXT_MENU_QUERY:
			if (handler->handle_context_menu_query_event_(dynamic_cast<context_menu_prefix &>(e)) && !default_prevented_of_(e))
				set_result_of_(e, 1, false);
			break;
		case WINP_WM_CONTEXT_MENU_REQUEST:
		{
			auto value = handler->handle_context_menu_request_event_(dynamic_cast<context_menu_prefix &>(e));
			if (value != nullptr && !default_prevented_of_(e))
				set_result_of_(e, reinterpret_cast<LRESULT>(value), false);
			break;
		}
		case WM_CONTEXTMENU:
			handler->handle_context_menu_event_(dynamic_cast<context_menu &>(e));
			if (default_prevented_of_(e))
				set_result_of_(e, 1, true);
			break;
		default:
			break;
		}
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}
