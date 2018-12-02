#include "../app/app_object.h"
#include "../window/frame_window.h"
#include "message_dispatcher.h"

winp::message::dispatcher::dispatcher()
	: event_dispatcher_(std::make_shared<event::dispatcher>()){}

winp::message::dispatcher::dispatcher(bool){}

LRESULT winp::message::dispatcher::dispatch_(ui::object &target, const MSG &info, bool call_default, unsigned int *states){
	return dispatch_(target, target, info, call_default, states);
}

LRESULT winp::message::dispatcher::dispatch_(ui::object &target, ui::object &context, const MSG &info, bool call_default, unsigned int *states){
	auto e = create_event_(target, context, info, call_default);
	return ((e == nullptr) ? 0 : dispatch_(*e, call_default, states));
}

LRESULT winp::message::dispatcher::dispatch_(event::object &e, bool call_default, unsigned int *states){
	e.state_ &= ~event::object::state_type::default_called;
	pre_dispatch_(e, call_default);

	if (!e.default_prevented_()){
		do_dispatch_(e, call_default);
		post_dispatch_(e);
	}

	if (states != nullptr)//Update states
		*states = e.state_;

	return e.get_result_();
}

void winp::message::dispatcher::pre_dispatch_(event::object &e, bool &call_default){}

void winp::message::dispatcher::post_dispatch_(event::object &e){}

void winp::message::dispatcher::do_dispatch_(event::object &e, bool call_default){
	fire_event_(e);
	if (e.default_prevented_())
		return;

	if (!e.default_done_())
		do_default_(e, call_default);
	else if (call_default && !e.default_called_())
		e.set_result_(call_default_(e), false);
}

void winp::message::dispatcher::do_default_(event::object &e, bool call_default){
	e.state_ |= event::object::state_type::default_done;
	event_dispatcher_->dispatch_(e);

	if (call_default && !e.default_prevented_() && !e.default_called_())
		e.set_result_(call_default_(e), false);
}

LRESULT winp::message::dispatcher::call_default_(event::object &e){
	e.state_ |= event::object::state_type::default_called;

	auto &info = *e.get_info();
	if (info.message >= WM_APP || e.context_ != e.target_)
		return 0;//APP message OR bubbled message

	return CallWindowProcW(get_default_message_entry_of_(*e.context_), info.hwnd, info.message, info.wParam, info.lParam);
}

void winp::message::dispatcher::fire_event_(event::object &e){}

std::shared_ptr<winp::event::object> winp::message::dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	return create_new_event_<event::object>(target, context, info, call_default);
}

winp::ui::object *winp::message::dispatcher::find_object_(HANDLE handle){
	return app::object::this_thread.surface_manager_.find_object_(handle);
}

std::size_t winp::message::dispatcher::event_handlers_count_of_(ui::object &target, event::manager_base &ev){
	return target.event_handlers_count_(ev);
}

void winp::message::dispatcher::fire_event_of_(ui::object &target, event::manager_base &ev, event::object &e){
	target.fire_event_(ev, e);
}

HANDLE winp::message::dispatcher::get_handle_of_(ui::object &target){
	return target.get_handle_();
}

WNDPROC winp::message::dispatcher::get_default_message_entry_of_(ui::object &target){
	return target.get_default_message_entry_();
}

std::list<winp::ui::object *> &winp::message::dispatcher::get_children_of_(ui::tree &target){
	return target.children_;
}

void winp::message::dispatcher::set_result_of_(event::object &e, LRESULT value, bool always_set){
	e.set_result_(value, always_set);
}

LRESULT winp::message::dispatcher::get_result_of_(event::object &e){
	return e.get_result_();
}

void winp::message::dispatcher::set_context_of_(event::object &e, ui::object &value){
	e.set_context_(value);
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

winp::message::tree_dispatcher::tree_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::tree_dispatcher>();
}

void winp::message::tree_dispatcher::fire_event_(event::object &e){
	auto tree_target = dynamic_cast<ui::tree *>(e.get_context());
	switch (e.get_info()->message){
	case WINP_WM_PARENT_CHANGING:
		fire_event_of_(*e.get_context(), e.get_context()->parent_change_event, e);
		break;
	case WINP_WM_INDEX_CHANGING:
		fire_event_of_(*e.get_context(), e.get_context()->index_change_event, e);
		break;
	case WINP_WM_CHILD_INDEX_CHANGING:
		if (tree_target != nullptr)
			fire_event_of_(*tree_target, tree_target->child_index_change_event, e);
		break;
	case WINP_WM_CHILD_INSERTING:
		if (tree_target != nullptr)
			fire_event_of_(*tree_target, tree_target->child_insert_event, e);
		break;
	case WINP_WM_CHILD_REMOVING:
		if (tree_target != nullptr)
			fire_event_of_(*tree_target, tree_target->child_remove_event, e);
		break;
	case WINP_WM_PARENT_CHANGED:
		fire_event_of_(*e.get_context(), e.get_context()->parent_changed_event, e);
		break;
	case WINP_WM_INDEX_CHANGED:
		fire_event_of_(*e.get_context(), e.get_context()->index_changed_event, e);
		break;
	case WINP_WM_CHILD_INDEX_CHANGED:
		if (tree_target != nullptr)
			fire_event_of_(*tree_target, tree_target->child_index_changed_event, e);
		break;
	case WINP_WM_CHILD_INSERTED:
		if (tree_target != nullptr)
			fire_event_of_(*tree_target, tree_target->child_inserted_event, e);
		break;
	case WINP_WM_CHILD_REMOVED:
		if (tree_target != nullptr)
			fire_event_of_(*tree_target, tree_target->child_removed_event, e);
		break;
	default:
		break;
	}
}

std::shared_ptr<winp::event::object> winp::message::tree_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	return create_new_event_<event::tree>(target, context, info, call_default);
}

winp::message::create_destroy_dispatcher::create_destroy_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::create_destroy_dispatcher>();
}

void winp::message::create_destroy_dispatcher::fire_event_(event::object &e){
	auto window_target = dynamic_cast<ui::window_surface *>(e.get_context());
	if (window_target == nullptr){//Try menu
		auto menu_target = dynamic_cast<menu::object *>(e.get_context());
		if (menu_target != nullptr){//Window or menu target is required
			if (e.get_info()->message == WM_CREATE)
				fire_event_of_(*menu_target, menu_target->create_event, e);
			else
				fire_event_of_(*menu_target, menu_target->destroy_event, e);
		}
	}
	else if (e.get_info()->message == WM_CREATE)
		fire_event_of_(*window_target, window_target->create_event, e);
	else
		fire_event_of_(*window_target, window_target->destroy_event, e);
}

winp::message::draw_dispatcher::draw_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::draw_dispatcher>();
}

void winp::message::draw_dispatcher::fire_event_(event::object &e){
	auto visible_target = dynamic_cast<ui::visible_surface *>(e.get_context());
	if (visible_target == nullptr)
		return;//Visible target required

	if (e.get_info()->message == WM_ERASEBKGND)
		fire_event_of_(*visible_target, visible_target->background_erase_event, e);
	else
		fire_event_of_(*visible_target, visible_target->draw_event, e);
}

std::shared_ptr<winp::event::object> winp::message::draw_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	return create_new_event_<event::draw>(target, context, info, call_default);
}

winp::message::draw_item_dispatcher::draw_item_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::draw_item_dispatcher>();
}

void winp::message::draw_item_dispatcher::fire_event_(event::object &e){
	auto menu_target = dynamic_cast<menu::object *>(e.get_context());
	auto menu_item_target = ((menu_target == nullptr) ? dynamic_cast<menu::item_component *>(e.get_context()) : nullptr);
	auto window_target = ((menu_target == nullptr && menu_item_target == nullptr) ? dynamic_cast<ui::window_surface *>(e.get_context()) : nullptr);

	if (e.get_info()->message == WM_DRAWITEM){
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->draw_item_event, e);
		else if (menu_item_target != nullptr)
			fire_event_of_(*menu_item_target, menu_item_target->draw_item_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->draw_menu_item_event, e);
	}
	else if (e.get_info()->message == WM_MEASUREITEM){
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->measure_item_event, e);
		else if (menu_item_target != nullptr)
			fire_event_of_(*menu_item_target, menu_item_target->measure_item_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->measure_menu_item_event, e);
	}

	if (default_prevented_of_(e))
		set_result_of_(e, 1, false);
}

std::shared_ptr<winp::event::object> winp::message::draw_item_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	if (info.message == WM_DRAWITEM)
		return create_new_event_<event::draw_item>(target, context, info, call_default);

	if (info.message == WM_MEASUREITEM)
		return create_new_event_<event::measure_item>(target, context, info, call_default);

	return nullptr;
}

winp::message::cursor_dispatcher::cursor_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::cursor_dispatcher>();
}

LRESULT winp::message::cursor_dispatcher::call_default_(event::object &e){
	return reinterpret_cast<LRESULT>(get_default_cursor_(dynamic_cast<event::cursor &>(e)));
}

void winp::message::cursor_dispatcher::fire_event_(event::object &e){
	auto surface_target = dynamic_cast<ui::io_surface *>(e.get_context());
	if (surface_target != nullptr)//IO target is required
		fire_event_of_(*surface_target, surface_target->set_cursor_event, e);
}

std::shared_ptr<winp::event::object> winp::message::cursor_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	return create_new_event_<event::cursor>(target, context, info, call_default);
}

HCURSOR winp::message::cursor_dispatcher::get_default_cursor_(event::cursor &e) const{
	switch (e.get_hit_target()){
	case HTERROR://Play beep if applicable
		switch (e.get_mouse_button()){
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
			MessageBeep(0);
			break;
		default:
			break;
		}

		return nullptr;
	case HTCLIENT://Use class cursor
		return reinterpret_cast<HCURSOR>(GetClassLongPtrW(static_cast<HWND>(get_handle_of_(*e.get_context())), GCLP_HCURSOR));
	case HTLEFT:
	case HTRIGHT:
		return LoadCursorW(nullptr, IDC_SIZEWE);
	case HTTOP:
	case HTBOTTOM:
		return LoadCursorW(nullptr, IDC_SIZENS);
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		return LoadCursorW(nullptr, IDC_SIZENWSE);
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		return LoadCursorW(nullptr, IDC_SIZENESW);
	default:
		break;
	}

	return LoadCursorW(nullptr, IDC_ARROW);
}

winp::message::mouse_dispatcher::mouse_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::mouse_dispatcher>();
}

void winp::message::mouse_dispatcher::fire_event_(event::object &e){
	auto manager = get_event_manager_(e);
	if (manager != nullptr)
		fire_event_of_(*dynamic_cast<ui::io_surface *>(e.get_context()), *manager, e);
}

std::shared_ptr<winp::event::object> winp::message::mouse_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	auto mouse_position = GetMessagePos();
	POINT computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };
	return create_new_event_<event::mouse>(target, context, info, call_default, computed_mouse_position, get_button_(info));
}

winp::event::mouse::button_type winp::message::mouse_dispatcher::get_button_(const MSG &info){
	switch (info.message){
	case WM_NCLBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_LBUTTONUP:
	case WM_NCLBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		return event::mouse::button_type::left;
	case WM_NCMBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		return event::mouse::button_type::middle;
	case WM_NCRBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_RBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		return event::mouse::button_type::right;
	default:
		break;
	}

	return event::mouse::button_type::nil;
}

winp::event::manager_base *winp::message::mouse_dispatcher::get_event_manager_(event::object &e){
	auto surface_target = dynamic_cast<ui::io_surface *>(e.get_context());
	if (surface_target == nullptr)//IO surface required
		return nullptr;

	switch (e.get_info()->message){
	case WINP_WM_MOUSELEAVE:
		return &surface_target->mouse_event.leave;
	case WINP_WM_MOUSEENTER:
		return &surface_target->mouse_event.enter;
	case WINP_WM_MOUSEMOVE:
		return &surface_target->mouse_event.move;
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		return &surface_target->mouse_event.wheel;
	case WM_NCLBUTTONDOWN:
	case WM_LBUTTONDOWN:
		return &surface_target->mouse_event.down;
	case WM_NCMBUTTONDOWN:
	case WM_MBUTTONDOWN:
		return &surface_target->mouse_event.down;
	case WM_NCRBUTTONDOWN:
	case WM_RBUTTONDOWN:
		return &surface_target->mouse_event.down;
	case WM_NCLBUTTONUP:
	case WM_LBUTTONUP:
		return &surface_target->mouse_event.up;
	case WM_NCMBUTTONUP:
	case WM_MBUTTONUP:
		return &surface_target->mouse_event.up;
	case WM_NCRBUTTONUP:
	case WM_RBUTTONUP:
		return &surface_target->mouse_event.up;
	case WM_NCLBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		return &surface_target->mouse_event.double_click;
	case WM_NCMBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		return &surface_target->mouse_event.double_click;
	case WM_NCRBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		return &surface_target->mouse_event.double_click;
	case WINP_WM_MOUSEDRAG:
		return &surface_target->mouse_event.drag;
	case WINP_WM_MOUSEDRAGBEGIN:
		return &surface_target->mouse_event.drag_begin;
	case WINP_WM_MOUSEDRAGEND:
		return &surface_target->mouse_event.drag_end;
	default:
		break;
	}

	return nullptr;
}

winp::message::focus_dispatcher::focus_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::focus_dispatcher>();
}

void winp::message::focus_dispatcher::fire_event_(event::object &e){
	auto surface_target = dynamic_cast<ui::io_surface *>(e.get_context());
	if (surface_target == nullptr)
		return;//IO target is required

	if (e.get_info()->message == WM_SETFOCUS)
		fire_event_of_(*surface_target, surface_target->set_focus_event, e);
	else
		fire_event_of_(*surface_target, surface_target->kill_focus_event, e);
}

winp::message::key_dispatcher::key_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::key_dispatcher>();
}

void winp::message::key_dispatcher::fire_event_(event::object &e){
	auto manager = get_event_manager_(e);
	if (manager != nullptr)
		fire_event_of_(*dynamic_cast<ui::io_surface *>(e.get_context()), *manager, e);
}

std::shared_ptr<winp::event::object> winp::message::key_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	return create_new_event_<event::key>(target, context, info, call_default);
}

winp::event::manager_base *winp::message::key_dispatcher::get_event_manager_(event::object &e){
	auto surface_target = dynamic_cast<ui::io_surface *>(e.get_context());
	if (surface_target == nullptr)//IO surface required
		return nullptr;

	switch (e.get_info()->message){
	case WM_KEYDOWN:
		return &surface_target->key_event.down;
	case WM_KEYUP:
		return &surface_target->key_event.up;
	case WM_CHAR:
		return &surface_target->key_event.press;
	default:
		break;
	}

	return nullptr;
}

winp::message::menu_dispatcher::menu_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::menu_dispatcher>();
}

void winp::message::menu_dispatcher::fire_event_(event::object &e){
	auto menu_target = dynamic_cast<menu::object *>(e.get_context());
	auto menu_item_target = ((menu_target == nullptr) ? dynamic_cast<menu::item *>(e.get_context()) : nullptr);
	auto window_target = ((menu_target == nullptr && menu_item_target == nullptr) ? dynamic_cast<ui::window_surface *>(e.get_context()) : nullptr);

	switch (e.get_info()->message){
	case WM_UNINITMENUPOPUP:
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->uninit_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->menu_uninit_event, e);
		break;
	case WM_INITMENUPOPUP:
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->init_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->menu_init_event, e);
		break;
	case WINP_WM_MENU_INIT_ITEM:
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->init_item_event, e);
		else if (menu_item_target != nullptr)
			fire_event_of_(*menu_item_target, menu_item_target->init_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->menu_init_item_event, e);
		break;
	case WINP_WM_MENU_SELECT:
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->select_event, e);
		else if (menu_item_target != nullptr)
			fire_event_of_(*menu_item_target, menu_item_target->select_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->menu_select_event, e);
		break;
	case WINP_WM_MENU_CHECK:
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->check_event, e);
		else if (menu_item_target != nullptr)
			fire_event_of_(*menu_item_target, menu_item_target->check_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->menu_check_event, e);
		break;
	case WINP_WM_MENU_UNCHECK:
		if (menu_target != nullptr)
			fire_event_of_(*menu_target, menu_target->uncheck_event, e);
		else if (menu_item_target != nullptr)
			fire_event_of_(*menu_item_target, menu_item_target->uncheck_event, e);
		else if (window_target != nullptr)
			fire_event_of_(*window_target, window_target->menu_uncheck_event, e);
		break;
	default:
		break;
	}
}

std::shared_ptr<winp::event::object> winp::message::menu_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	if (info.message == WINP_WM_MENU_INIT_ITEM || info.message == WINP_WM_MENU_SELECT || info.message == WINP_WM_MENU_CHECK || info.message == WINP_WM_MENU_UNCHECK){
		if (auto item = reinterpret_cast<menu::item_component *>(info.wParam); item != nullptr)
			return create_new_event_<event::object>(*item, *item, info, call_default);
		return nullptr;
	}

	if (info.message == WINP_WM_CONTEXT_MENU_QUERY || info.message == WINP_WM_CONTEXT_MENU_REQUEST)
		return create_new_event_<event::context_menu_prefix>(target, context, info, call_default);

	if (info.message == WM_CONTEXTMENU){
		auto context_targets = reinterpret_cast<thread::surface_manager::context_menu_targets_info *>(info.wParam);
		if (context_targets->surface == nullptr)
			return create_new_event_<event::context_menu>(target, context, MSG{ info.hwnd, info.message, reinterpret_cast<WPARAM>(context_targets->menu), info.lParam }, call_default);
		return create_new_event_<event::context_menu>(*context_targets->surface, target, MSG{ info.hwnd, info.message, reinterpret_cast<WPARAM>(context_targets->menu), info.lParam }, call_default);
	}

	auto menu_target = find_object_(reinterpret_cast<HMENU>(info.wParam));
	if (menu_target == nullptr)
		return create_new_event_<event::object>(target, context, info, call_default);

	return create_new_event_<event::object>(*menu_target, *menu_target, info, call_default);
}

winp::message::frame_dispatcher::frame_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::frame_dispatcher>();
}

void winp::message::frame_dispatcher::fire_event_(event::object &e){
	auto surface_target = dynamic_cast<ui::surface *>(e.get_context());
	if (surface_target == nullptr)
		return;

	auto frame_window_target = dynamic_cast<window::frame *>(e.get_context());
	switch (e.get_info()->message){
	case WM_CLOSE:
		if (frame_window_target != nullptr)
			fire_event_of_(*frame_window_target, frame_window_target->close_event, e);
		break;
	case WM_SIZING:
		fire_event_of_(*surface_target, surface_target->size_change_event, e);
		break;
	case WM_MOVING:
		fire_event_of_(*surface_target, surface_target->position_change_event, e);
		break;
	case WM_SIZE:
		switch (e.get_info()->wParam){
		case SIZE_MAXIMIZED:
			if (frame_window_target != nullptr)
				fire_event_of_(*frame_window_target, frame_window_target->maximized_event, e);
			break;
		case SIZE_MINIMIZED:
			if (frame_window_target != nullptr)
				fire_event_of_(*frame_window_target, frame_window_target->minimized_event, e);
			break;
		case SIZE_RESTORED:
			break;
		default:
			return;
		}
		fire_event_of_(*surface_target, surface_target->size_changed_event, e);
		break;
	case WM_MOVE:
		fire_event_of_(*surface_target, surface_target->position_changed_event, e);
		break;
	default:
		break;
	}
}

std::shared_ptr<winp::event::object> winp::message::frame_dispatcher::create_event_(ui::object &target, ui::object &context, const MSG &info, bool call_default){
	switch (info.message){
	case WM_SIZING:
		return create_new_event_<event::size>(target, context, info, call_default);
	case WM_MOVING:
		return create_new_event_<event::position>(target, context, info, call_default);
	default:
		break;
	}

	return create_new_event_<event::object>(target, context, info, call_default);
}
