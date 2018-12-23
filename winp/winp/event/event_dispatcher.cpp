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
	if (handler == nullptr)
		return dispatcher::dispatch_(e);

	switch (e.get_info()->message){
	case WINP_WM_PARENT_CHANGING:
		e.set_result((handler->handle_parent_change_event_(dynamic_cast<tree &>(e)) ? 0 : 1), true);
		break;
	case WINP_WM_INDEX_CHANGING:
		e.set_result((handler->handle_index_change_event_(dynamic_cast<tree &>(e)) ? 0 : 1), true);
		break;
	case WINP_WM_CHILD_INDEX_CHANGING:
		e.set_result((handler->handle_child_index_change_event_(dynamic_cast<tree &>(e)) ? 0 : 1), true);
		break;
	case WINP_WM_CHILD_INSERTING:
		e.set_result((handler->handle_child_insert_event_(dynamic_cast<tree &>(e)) ? 0 : 1), true);
		break;
	case WINP_WM_CHILD_REMOVING:
		e.set_result((handler->handle_child_remove_event_(dynamic_cast<tree &>(e)) ? 0 : 1), true);
		break;
	case WINP_WM_PARENT_CHANGED:
		handler->handle_parent_changed_event_(dynamic_cast<tree &>(e));
		break;
	case WINP_WM_INDEX_CHANGED:
		handler->handle_index_changed_event_(dynamic_cast<tree &>(e));
		break;
	case WINP_WM_CHILD_INDEX_CHANGED:
		handler->handle_child_index_changed_event_(dynamic_cast<tree &>(e));
		break;
	case WINP_WM_CHILD_INSERTED:
		handler->handle_child_inserted_event_(dynamic_cast<tree &>(e));
		break;
	case WINP_WM_CHILD_REMOVED:
		handler->handle_child_removed_event_(dynamic_cast<tree &>(e));
		break;
	default:
		break;
	}
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
		case WINP_WM_ERASE_BACKGROUND:
		case WINP_WM_ERASE_NON_CLIENT_BACKGROUND:
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
	else if (dynamic_cast<unhandled_handler *>(e.get_context()) == nullptr){
		switch (e.get_info()->message){
		case WM_ERASEBKGND:
		case WINP_WM_ERASE_BACKGROUND:
		case WINP_WM_ERASE_NON_CLIENT_BACKGROUND:
			erase_background_(dynamic_cast<draw &>(e));
			break;
		case WM_PAINT:
		case WM_PRINTCLIENT:
			dynamic_cast<draw &>(e).get_device_();//Trigger paint begin
			break;
		default:
			break;
		}
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::draw_dispatcher::erase_background_(draw &e){
	auto visible_surface = dynamic_cast<ui::visible_surface *>(e.get_context());
	if (visible_surface != nullptr && !visible_surface->is_transparent_()){
		auto drawer = e.get_drawer_();
		if (drawer != nullptr && e.info_.message == WINP_WM_ERASE_NON_CLIENT_BACKGROUND)
			drawer->Clear(dynamic_cast<non_window::child *>(visible_surface)->non_client_background_color_);
		else if (drawer != nullptr)
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
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::draw_item_dispatcher::draw_item_(draw_item &e){
	e.draw();
	e.stop_propagation();
	e.prevent_default();
}

void winp::event::draw_item_dispatcher::measure_item_(measure_item &e){
	e.set_size(e.get_measured_size_());
	e.stop_propagation();
	e.prevent_default();
}

void winp::event::draw_item_dispatcher::draw_item_(ui::object &item, DRAWITEMSTRUCT &info, HWND handle, HTHEME theme){
	HTHEME created_theme = nullptr;
	if (theme == nullptr){
		auto theme_name = item.get_theme_name();
		if (theme_name == nullptr)
			return;

		created_theme = OpenThemeData(nullptr, theme_name);
		theme = created_theme;
	}

	auto menu_item = dynamic_cast<menu::item_component *>(&item);
	if (menu_item != nullptr){
		if (theme == nullptr)
			draw_unthemed_menu_item_(*menu_item, info, handle);
		else//No theme
			draw_themed_menu_item_(*menu_item, info, handle, theme);
	}

	if (created_theme != nullptr)
		CloseThemeData(created_theme);
}

void winp::event::draw_item_dispatcher::draw_themed_menu_item_(menu::item_component &item, DRAWITEMSTRUCT &info, HWND handle, HTHEME theme){
	if (dynamic_cast<menu::separator *>(&item) == nullptr){
		auto label = item.get_label_();
		if (item.is_popup_item_()){
			int item_state_id = 0, sub_state_id = 0, check_bk_state_id = 0, check_state_id = 0;
			auto check_item = dynamic_cast<menu::check_item *>(&item);
			auto popup = item.get_popup_();

			auto is_disabled = ((info.itemState & ODS_DISABLED) != 0u);
			auto is_checked = (check_item != nullptr && (info.itemState & ODS_CHECKED) != 0u);
			auto has_sub_menu = (popup != nullptr && popup->get_handle() != nullptr);

			DrawThemeBackground(theme, info.hDC, MENU_POPUPBACKGROUND, 0, &info.rcItem, nullptr);
			if ((info.itemState & ODS_SELECTED) == 0u)//Not selected
				item_state_id = (is_disabled ? MPI_DISABLED : MPI_NORMAL);
			else//Selected item
				item_state_id = (is_disabled ? MPI_DISABLEDHOT : MPI_HOT);

			sub_state_id = (is_disabled ? MSM_DISABLED : MSM_NORMAL);
			if (is_checked){
				check_bk_state_id = (is_disabled ? MCB_DISABLED : MCB_NORMAL);

				if (check_item->is_radio_())
					check_state_id = (is_disabled ? MC_BULLETDISABLED : MC_BULLETNORMAL);
				else
					check_state_id = (is_disabled ? MC_CHECKMARKDISABLED : MC_CHECKMARKNORMAL);
			}

			DrawThemeBackground(theme, info.hDC, MENU_POPUPITEM, item_state_id, &info.rcItem, nullptr);
			if (has_sub_menu){//Draw sub-menu caret
				RECT sub_rect{ (info.rcItem.right - get_menu_item_text_padding_(info.hDC, theme)), info.rcItem.top, info.rcItem.right, info.rcItem.bottom };
				DrawThemeBackground(theme, info.hDC, MENU_POPUPSUBMENU, sub_state_id, &sub_rect, nullptr);
			}

			auto check_extent = get_menu_item_check_extent_(info.hDC, theme);
			auto check_gutter = get_menu_item_check_gutter_(theme);

			if (is_checked){//Draw check box
				RECT check_rect{ info.rcItem.left, info.rcItem.top, (info.rcItem.left + check_extent.cx), info.rcItem.bottom };
				DrawThemeBackground(theme, info.hDC, MENU_POPUPCHECKBACKGROUND, check_bk_state_id, &check_rect, nullptr);
				DrawThemeBackground(theme, info.hDC, MENU_POPUPCHECK, check_state_id, &check_rect, nullptr);
			}

			if (label != nullptr && !label->empty()){//Draw label
				RECT text_rect{ (info.rcItem.left + check_extent.cx + check_gutter), info.rcItem.top, info.rcItem.right, info.rcItem.bottom };
				DrawThemeText(theme, info.hDC, MENU_POPUPITEM, item_state_id, label->data(), static_cast<int>(label->size()), (DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE), 0u, &text_rect);
			}
		}
		else if ((info.itemState & ODS_DISABLED) == 0u){//Enabled bar item
			if ((info.itemState & ODS_SELECTED) == 0u){//Not selected
				
			}
			else{//Selected item
				
			}
		}
		else{//Disabled bar item
			
		}
	}
	else//Separator
		DrawThemeBackground(theme, info.hDC, MENU_POPUPSEPARATOR, 0, &info.rcItem, nullptr);
}

void winp::event::draw_item_dispatcher::draw_unthemed_menu_item_(menu::item_component &item, DRAWITEMSTRUCT &info, HWND handle){
	if (dynamic_cast<menu::separator *>(&item) == nullptr){
		DWORD text_color = 0u;
		HBRUSH backgroud_brush = nullptr;

		if (item.is_popup_item_()){
			if ((info.itemState & ODS_DISABLED) == 0u){
				if ((info.itemState & ODS_SELECTED) == 0u){
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
		else if ((info.itemState & ODS_DISABLED) == 0u){//Enabled bar item
			if ((info.itemState & ODS_SELECTED) == 0u){
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
			FillRect(info.hDC, &info.rcItem, backgroud_brush);

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

			auto old_font = SelectObject(info.hDC, ((bold_font == nullptr) ? font : bold_font));
			auto old_text_color = SetTextColor(info.hDC, text_color);
			auto old_background_mode = SetBkMode(info.hDC, TRANSPARENT);

			OffsetRect(&info.rcItem, (GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE)), 0);
			DrawTextW(info.hDC, label->data(), static_cast<int>(label->size()), &info.rcItem, (DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE));

			SetBkMode(info.hDC, old_background_mode);//Restore background mode
			SetTextColor(info.hDC, old_text_color);//Restore text color
			SelectObject(info.hDC, old_font);//Restore font

			if (bold_font != nullptr)//Destroy created font
				DeleteObject(bold_font);
		}
	}
}

SIZE winp::event::draw_item_dispatcher::measure_item_(ui::object &item, HWND handle, HDC device, HTHEME theme){
	HDC created_device = nullptr;
	HTHEME created_theme = nullptr;

	if (device == nullptr){
		created_device = GetDC(handle);
		if ((device = created_device) == nullptr)//Failed to retrieve device
			return SIZE{};
	}
	
	if (theme == nullptr){
		auto theme_name = item.get_theme_name();
		if (theme_name == nullptr)
			return SIZE{};

		created_theme = OpenThemeData(nullptr, theme_name);
		theme = created_theme;
	}

	auto menu_item_target = dynamic_cast<menu::item_component *>(&item);
	auto control_target = dynamic_cast<control::object *>(&item);

	SIZE result{};
	if (menu_item_target != nullptr){
		auto label = menu_item_target->get_label_();
		result = control::object::compute_size(handle, device, menu_item_target->get_font_(), ((label == nullptr) ? L"" : *label));
	}
	else if (control_target != nullptr)
		result = control_target->get_computed_size_();

	if (menu_item_target != nullptr){
		if (menu_item_target->is_popup_item_()){
			auto check_extent = get_menu_item_check_extent_(device, theme);
			auto check_gutter = get_menu_item_check_gutter_(theme);
			auto text_padding = get_menu_item_text_padding_(device, theme);

			result = SIZE{ (result.cx + check_extent.cx + check_gutter + text_padding), (((result.cy < check_extent.cy) ? check_extent.cy : result.cy) + 6) };
		}
		else//Bar item
			result = SIZE{ (result.cx + 6), (result.cy + 6) };
	}

	if (created_device != nullptr)
		ReleaseDC(handle, created_device);

	if (created_theme != nullptr)
		CloseThemeData(created_theme);

	return result;
}

int winp::event::draw_item_dispatcher::get_menu_item_text_offset_(HDC device, HTHEME theme){
	return (get_menu_item_check_extent_(device, theme).cx + get_menu_item_check_gutter_(theme));
}

int winp::event::draw_item_dispatcher::get_menu_item_text_padding_(HDC device, HTHEME theme){
	if (theme == nullptr)
		return 4;

	SIZE check_size{};
	GetThemePartSize(theme, device, MENU_POPUPSUBMENU, MSM_NORMAL, nullptr, THEMESIZE::TS_DRAW, &check_size);

	return (check_size.cx * 2);
}

SIZE winp::event::draw_item_dispatcher::get_menu_item_check_extent_(HDC device, HTHEME theme){
	if (theme == nullptr)
		return SIZE{ GetSystemMetrics(SM_CXMENUCHECK), GetSystemMetrics(SM_CYMENUCHECK) };

	SIZE check_size{};
	GetThemePartSize(theme, device, MENU_POPUPCHECK, MC_CHECKMARKNORMAL, nullptr, THEMESIZE::TS_DRAW, &check_size);
	check_size.cx += 6;

	return check_size;
}

int winp::event::draw_item_dispatcher::get_menu_item_check_gutter_(HTHEME theme){
	return ((theme == nullptr) ? GetSystemMetrics(SM_CXEDGE) : (GetSystemMetrics(SM_CXMENUCHECK) - 6));
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
		case WINP_WM_CONTEXT_MENU_REQUEST:
		{
			auto value = handler->handle_context_menu_request_event_(dynamic_cast<context_menu_prefix &>(e));
			if (value != nullptr)
				set_result_of_(e, reinterpret_cast<LRESULT>(value), false);
			break;
		}
		case WM_CONTEXTMENU:
			handler->handle_context_menu_event_(dynamic_cast<context_menu &>(e));
			break;
		default:
			break;
		}
	}
	else//Events are not subscribed to
		dispatcher::dispatch_(e);
}

void winp::event::frame_dispatcher::dispatch_(object &e){
	auto handler = dynamic_cast<frame_handler *>(e.get_context());
	if (handler == nullptr)
		return dispatcher::dispatch_(e);

	switch (e.get_info()->message){
	case WM_CLOSE:
		if (!handler->handle_close_event_(e))
			e.prevent_default();
		break;
	case WM_SIZING:
		if (!handler->handle_size_change_event_(dynamic_cast<event::size &>(e)))
			e.prevent_default();
		break;
	case WM_MOVING:
		if (!handler->handle_position_change_event_(dynamic_cast<event::position &>(e)))
			e.prevent_default();
		break;
	case WM_SIZE:
		switch (e.get_info()->wParam){
		case SIZE_MAXIMIZED:
			handler->handle_maximized_event_(e);
			break;
		case SIZE_MINIMIZED:
			handler->handle_minimized_event_(e);
			break;
		case SIZE_RESTORED:
			break;
		default:
			return;
		}
		handler->handle_size_changed_event_(e);
		break;
	case WM_MOVE:
		handler->handle_position_changed_event_(e);
		break;
	default:
		break;
	}
}
