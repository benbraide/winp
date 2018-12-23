#include "../window/frame_window.h"
#include "../message/message_dispatcher.h"
#include "../app/app_object.h"

winp::thread::surface_manager::surface_manager() = default;

void winp::thread::surface_manager::prepare_for_run_(){
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, GetCurrentThreadId());
}

bool winp::thread::surface_manager::is_dialog_message_(MSG &msg) const{
	if (state_.focused != nullptr)
		return state_.focused->is_dialog_message_(msg);
	return ((state_.mouse_focused == nullptr) ? false : state_.mouse_focused->is_dialog_message_(msg));
}

void winp::thread::surface_manager::translate_message_(MSG &msg) const{
	TranslateMessage(&msg);
}

void winp::thread::surface_manager::dispatch_message_(MSG &msg) const{
	if (msg.hwnd == nullptr || !is_dialog_message_(msg)){
		translate_message_(msg);
		DispatchMessageW(&msg);
	}
}

winp::ui::surface *winp::thread::surface_manager::find_object_(HANDLE handle) const{
	if (handle == cache_.handle)
		return cache_.object;

	if (map_.empty())
		return nullptr;

	auto it = map_.find(handle);
	if (it == map_.end())
		return nullptr;

	if (IsWindow(static_cast<HWND>(handle)) != FALSE){
		cache_.handle = handle;
		cache_.object = it->second;
	}

	return it->second;
}

winp::ui::surface *winp::thread::surface_manager::find_item_(UINT id) const{
	if (id_map_.empty())
		return nullptr;

	auto it = id_map_.find(id);
	return ((it == id_map_.end()) ? nullptr : it->second);
}

winp::menu::item_component *winp::thread::surface_manager::find_system_menu_item_(ui::surface &target, UINT id) const{
	if (auto frame_target = dynamic_cast<window::frame *>(&target); frame_target != nullptr)
		return frame_target->get_system_menu_()->find_component_(id, nullptr);
	return nullptr;
}

void winp::thread::surface_manager::create_window_(HWND handle, CBT_CREATEWNDW &info){
	if (cache_.creating == nullptr || static_cast<ui::surface *>(info.lpcs->lpCreateParams) != cache_.creating)
		return;//External source

	cache_.handle = handle;
	cache_.object = cache_.creating;

	map_[handle] = cache_.creating;
	cache_.creating = nullptr;

	cache_.object->set_handle_(handle);
	cache_.object->set_message_entry_(reinterpret_cast<LONG_PTR>(entry_));
	cache_.object->add_to_toplevel_();

	auto frame_object = dynamic_cast<window::frame *>(cache_.object);
	if (frame_object != nullptr)//Update system menu
		frame_object->system_menu_.init_(GetSystemMenu(handle, FALSE));

	static_cast<ui::surface *>(info.lpcs->lpCreateParams)->call_hook_(ui::hook::parent_size_change_hook_code);
}

LRESULT winp::thread::surface_manager::destroy_window_(ui::surface &target, const MSG &info){
	if (!toplevel_map_.empty())
		toplevel_map_.erase(info.hwnd);

	if (!map_.empty())
		map_.erase(info.hwnd);

	if (cache_.handle == info.hwnd){
		cache_.handle = nullptr;
		cache_.object = nullptr;
	}

	auto frame_object = dynamic_cast<window::frame *>(&target);
	if (frame_object != nullptr)//Destroy system menu
		frame_object->system_menu_.destroy_();

	return find_dispatcher_(info.message)->dispatch_(target, info, true, nullptr);
}

LRESULT winp::thread::surface_manager::draw_(ui::surface &target, const MSG &info, bool prevent_default, m_rect_type update_region){
	if (info.message == WINP_WM_PAINT){//Erase background
		if (auto non_window_target = dynamic_cast<non_window::child *>(&target); non_window_target != nullptr && non_window_target->non_client_handle_ != nullptr)
			find_dispatcher_(WINP_WM_ERASE_NON_CLIENT_BACKGROUND)->dispatch_(target, MSG{ info.hwnd, WINP_WM_ERASE_NON_CLIENT_BACKGROUND, info.wParam, info.lParam }, !prevent_default, nullptr);
		find_dispatcher_(WINP_WM_ERASE_BACKGROUND)->dispatch_(target, MSG{ info.hwnd, WINP_WM_ERASE_BACKGROUND, info.wParam, info.lParam }, !prevent_default, nullptr);
	}
	
	if (dynamic_cast<ui::window_surface *>(&target) == nullptr)
		find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);
	else//Get clip
		find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr, &update_region);

	ui::visible_surface *visible_child = nullptr;
	for (auto child : target.children_){
		if (dynamic_cast<ui::window_surface *>(child) == nullptr && (visible_child = dynamic_cast<ui::visible_surface *>(child)) != nullptr && visible_child->is_created_())
			draw_(*visible_child, MSG{ info.hwnd, WINP_WM_PAINT, reinterpret_cast<WPARAM>(&update_region), info.lParam }, true, update_region);
	}

	return 0;
}

LRESULT winp::thread::surface_manager::mouse_nc_leave_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	mouse_info_.tracking_mouse = false;

	if (target.hit_test_(m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) }, true) != HTCLIENT)
		find_dispatcher_(info.message)->dispatch_(target, info, false, nullptr);//Left window

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_leave_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	mouse_info_.tracking_mouse = false;

	if (target.hit_test_(m_point_type{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) }, true) != HTCLIENT)
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);//Left window
	
	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_nc_move_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	if (!mouse_info_.tracking_mouse)
		track_mouse_leave_(static_cast<HWND>(target.get_handle_()), TME_NONCLIENT);

	mouse_move_(target, info, mouse_position, true);
	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_move_(ui::window_surface &target, const MSG &info, DWORD mouse_position, bool prevent_default){
	if (!mouse_info_.tracking_mouse)
		track_mouse_leave_(static_cast<HWND>(target.get_handle_()), 0);

	if (mouse_info_.mouse_target != &target){//Mouse enter
		find_dispatcher_(WINP_WM_MOUSEENTER)->dispatch_(target, MSG{ info.hwnd, WINP_WM_MOUSEENTER, info.wParam, info.lParam }, false, nullptr);
		mouse_info_.mouse_target = &target;
	}

	unsigned int states = 0;
	auto dispatcher = find_dispatcher_(WINP_WM_MOUSEMOVE);

	auto result = dispatcher->dispatch_(target, info, !prevent_default, &states);
	if ((states & event::object::state_type::propagation_stopped) != 0u)
		return result;//Propagation stopped

	for (auto ancestor = target.get_first_ancestor_of_<ui::window_surface>(); ancestor != nullptr; ancestor = ancestor->get_first_ancestor_of_<ui::window_surface>()){
		dispatcher->dispatch_(target, *ancestor, info, false, &states);
		if ((states & event::object::state_type::propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	return result;
}

LRESULT winp::thread::surface_manager::nc_mouse_other_(ui::window_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	mouse_other_(target, info, mouse_position, button, true);
	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::mouse_other_(ui::window_surface &target, const MSG &info, DWORD mouse_position, UINT button, bool prevent_default){
	state_.mouse_focused = &target;
	
	unsigned int states = 0;
	auto dispatcher = find_dispatcher_(info.message);

	auto result = dispatcher->dispatch_(target, info, !prevent_default, &states);
	if ((states & event::object::state_type::propagation_stopped) != 0u)
		return result;//Propagation stopped

	for (auto ancestor = target.get_first_ancestor_of_<ui::window_surface>(); ancestor != nullptr; ancestor = ancestor->get_first_ancestor_of_<ui::window_surface>()){
		dispatcher->dispatch_(target, *ancestor, info, false, &states);
		if ((states & event::object::state_type::propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	return result;
}

LRESULT winp::thread::surface_manager::set_cursor_(ui::window_surface &target, const MSG &info, bool prevent_default){
	auto value = find_dispatcher_(WM_SETCURSOR)->dispatch_(target, info, !prevent_default, nullptr);
	SetCursor((value == 0) ? LoadCursorW(nullptr, IDC_ARROW) : reinterpret_cast<HCURSOR>(value));
	return TRUE;
}

LRESULT winp::thread::surface_manager::set_focus_(ui::window_surface &target, const MSG &info, bool prevent_default){
	state_.focused = &target;
	return find_dispatcher_(WINP_WM_FOCUS)->dispatch_(target, info, !prevent_default, nullptr);
}

LRESULT winp::thread::surface_manager::kill_focus_(ui::window_surface &target, const MSG &info, bool prevent_default){
	if (state_.focused == &target)
		state_.focused = state_.mouse_focused;

	return find_dispatcher_(WINP_WM_FOCUS)->dispatch_(target, info, !prevent_default, nullptr);
}

LRESULT winp::thread::surface_manager::key_(ui::window_surface &target, const MSG &info, bool prevent_default){
	unsigned int states = 0;
	auto dispatcher = find_dispatcher_(WINP_WM_KEY);

	auto result = dispatcher->dispatch_(target, info, !prevent_default, &states);
	if ((states & event::object::state_type::propagation_stopped) != 0u)
		return result;//Propagation stopped

	for (auto ancestor = target.get_first_ancestor_of_<ui::window_surface>(); ancestor != nullptr; ancestor = ancestor->get_first_ancestor_of_<ui::window_surface>()){
		dispatcher->dispatch_(target, *ancestor, info, false, &states);
		if ((states & event::object::state_type::propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	return result;
}

LRESULT winp::thread::surface_manager::command_(ui::surface &target, const MSG &info, bool prevent_default){
	if (info.lParam == 0){//Accelerator | Menu
		if (HIWORD(info.wParam) != 1u){//Menu
			auto item = dynamic_cast<menu::item_component *>(find_item_(static_cast<UINT>(info.wParam)));
			if (item == nullptr)
				return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

			return menu_select_(target, info, *item, true);
		}
	}

	return 0;
}

LRESULT winp::thread::surface_manager::system_command_(ui::surface &target, const MSG &info, bool prevent_default){
	menu::item_component *item = nullptr;
	switch (info.wParam & 0xFFF0){
	case SC_CLOSE:
	case SC_CONTEXTHELP:
	case SC_DEFAULT:
	case SC_HOTKEY:
	case SC_HSCROLL:
	case SCF_ISSECURE:
	case SC_KEYMENU:
	case SC_MAXIMIZE:
	case SC_MINIMIZE:
	case SC_MONITORPOWER:
	case SC_MOUSEMENU:
	case SC_MOVE:
	case SC_NEXTWINDOW:
	case SC_PREVWINDOW:
	case SC_RESTORE:
	case SC_SCREENSAVE:
	case SC_SIZE:
	case SC_TASKLIST:
	case SC_VSCROLL:
		item = find_system_menu_item_(target, static_cast<UINT>(info.wParam & 0xFFF0));
		break;
	default:
		item = dynamic_cast<menu::item_component *>(find_item_(static_cast<UINT>(info.wParam)));
		break;
	}

	if (item == nullptr)
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

	return menu_select_(target, info, *item, prevent_default);
}

LRESULT winp::thread::surface_manager::menu_uninit_(ui::surface &target, const MSG &info, bool prevent_default){
	auto menu = dynamic_cast<menu::object *>(find_object_(reinterpret_cast<HMENU>(info.wParam)));
	if (menu == nullptr)
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

	unsigned int states = 0;
	auto dispatcher = find_dispatcher_(info.message);

	auto result = dispatcher->dispatch_(*menu, info, !prevent_default, &states);
	if ((states & event::object::state_type::propagation_stopped) == 0u)//Propagation not stopped
		dispatcher->dispatch_(*menu, target, info, false, &states);

	return result;
}

LRESULT winp::thread::surface_manager::menu_init_(ui::surface &target, const MSG &info, bool prevent_default){
	auto menu = dynamic_cast<menu::object *>(find_object_(reinterpret_cast<HMENU>(info.wParam)));
	if (menu == nullptr)
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

	unsigned int states = 0;
	auto dispatcher = find_dispatcher_(info.message);

	auto result = dispatcher->dispatch_(*menu, info, !prevent_default, &states);
	if ((states & event::object::state_type::propagation_stopped) == 0u)//Propagation not stopped
		dispatcher->dispatch_(*menu, target, info, false, &states);

	if ((states & event::object::state_type::default_prevented) == 0u)
		menu_init_items_(target, *menu);//Default not prevented

	return result;
}

LRESULT winp::thread::surface_manager::menu_init_items_(ui::surface &target, ui::surface &tree){
	menu::item *item;
	for (auto child : tree.children_){
		if (dynamic_cast<menu::tree *>(child) != nullptr)
			menu_init_items_(target, *dynamic_cast<ui::surface *>(child));
		else if ((item = dynamic_cast<menu::item *>(child)) != nullptr)
			menu_init_item_(target, *static_cast<menu::item_component *>(item));
	}

	return 0;
}

void winp::thread::surface_manager::menu_init_item_(ui::surface &target, menu::item_component &item){
	unsigned int states = 0;
	auto dispatcher = find_dispatcher_(WINP_WM_MENU_INIT_ITEM);

	MSG info{ static_cast<HWND>(target.get_handle_()), WINP_WM_MENU_INIT_ITEM };
	dispatcher->dispatch_(item, info, false, &states);

	if ((states & event::object::state_type::propagation_stopped) == 0u){//Propagation not stopped
		for (auto ancestor = item.get_first_ancestor_of_<menu::object, ui::window_surface>(); ancestor != nullptr; ancestor = ancestor->get_first_ancestor_of_<menu::object, ui::window_surface>()){
			dispatcher->dispatch_(item, *ancestor, info, false, &states);
			if ((states & event::object::state_type::propagation_stopped) != 0u)//Propagation stopped
				break;
		}
	}

	if ((states & event::object::state_type::propagation_stopped) == 0u)//Propagation not stopped
		dispatcher->dispatch_(item, target, info, false, &states);

	if ((states & event::object::state_type::default_prevented) == 0u)//Default not prevented
		item.remove_state_(MFS_DISABLED);
	else//Disabled
		item.set_state_(MFS_DISABLED);
}

LRESULT winp::thread::surface_manager::menu_select_(ui::surface &target, const MSG &info, bool prevent_default){
	auto menu = dynamic_cast<menu::object *>(find_object_(reinterpret_cast<HMENU>(info.lParam)));
	if (menu == nullptr)
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

	auto item = menu->get_component_at_absolute_index_(static_cast<std::size_t>(info.wParam));
	if (item == nullptr)
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

	return menu_select_(target, info, *item, prevent_default);
}

LRESULT winp::thread::surface_manager::menu_select_(ui::surface &target, const MSG &info, menu::item_component &item, bool prevent_default){
	unsigned int states = 0;
	auto selectable = dynamic_cast<menu::item *>(&item);

	if (selectable != nullptr)
		selectable->select_(&target, &info, prevent_default, states);

	if ((states & event::object::state_type::default_prevented) != 0u)
		return 0;//Default prevented

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::select_menu_item_(UINT msg, menu::item &item, ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states){
	auto dispatcher = find_dispatcher_(msg);
	dispatcher->dispatch_(item, MSG{ nullptr, msg }, false, &states);

	if ((states & event::object::state_type::propagation_stopped) == 0u){//Propagation not stopped
		for (auto ancestor = item.get_first_ancestor_of_<menu::object, ui::window_surface>(); ancestor != nullptr; ancestor = ancestor->get_first_ancestor_of_<menu::object, ui::window_surface>()){
			dispatcher->dispatch_(item, *ancestor, MSG{ nullptr, msg }, false, &states);
			if ((states & event::object::state_type::propagation_stopped) != 0u)//Propagation stopped
				break;
		}
	}

	if (target != nullptr && (states & event::object::state_type::propagation_stopped) == 0u)//Propagation not stopped
		dispatcher->dispatch_(item, *target, MSG{ nullptr, msg }, false, &states);

	return 0;
}

LRESULT winp::thread::surface_manager::context_menu_(ui::window_surface &target, const MSG &info, bool prevent_default){
	auto real_target = find_object_(reinterpret_cast<HWND>(info.wParam));
	if (real_target == nullptr)//No target window
		real_target = &target;

	POINT position{ GET_X_LPARAM(info.lParam), GET_Y_LPARAM(info.lParam) };
	if (position.x == -1 && position.y == -1)
		position = target.convert_position_to_absolute_value_(target.get_cursor_position_());

	unsigned int states = 0;
	auto request_result = reinterpret_cast<menu::object *>(find_dispatcher_(WINP_WM_CONTEXT_MENU_REQUEST)->dispatch_(*real_target, target, MSG{ info.hwnd, WINP_WM_CONTEXT_MENU_REQUEST, info.wParam, info.lParam }, false, &states));
	if ((states & event::object::state_type::default_prevented) != 0u)
		return 0;//Default not prevented

	if (request_result != nullptr){//Object returned a menu
		TrackPopupMenu(static_cast<HMENU>(request_result->get_handle_()), (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON), position.x, position.y, 0, static_cast<HWND>(target.get_handle_()), nullptr);
		return 0;
	}

	if ((cache_.context_menu = std::make_shared<menu::collection>()) == nullptr)//Error
		return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));

	cache_.context_menu->create_();
	context_menu_targets_info context_targets{
		find_object_(reinterpret_cast<HWND>(info.wParam)),
		cache_.context_menu.get()
	};

	find_dispatcher_(WM_CONTEXTMENU)->dispatch_(*real_target, target, MSG{ info.hwnd, info.message, reinterpret_cast<WPARAM>(&context_targets), info.lParam }, false, &states);
	if ((states & event::object::state_type::default_prevented) == 0u && !cache_.context_menu->children_.empty()){//Not rejected AND items added to menu
		TrackPopupMenu(static_cast<HMENU>(cache_.context_menu->get_handle_()), (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON), position.x, position.y, 0, info.hwnd, nullptr);
		return 0;
	}

	cache_.context_menu = nullptr;//Clear menu
	if ((states & event::object::state_type::propagation_stopped) != 0u)
		return 0;//Propagation stopped

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));//Rejected
}

LRESULT winp::thread::surface_manager::draw_item_(ui::surface &target, const MSG &info, bool prevent_default){
	auto pinfo = reinterpret_cast<DRAWITEMSTRUCT *>(info.lParam);
	auto dispatcher = find_dispatcher_(info.message);

	if (info.wParam != 0){

	}
	else if (auto target_item = dynamic_cast<menu::item_component *>(find_item_(pinfo->itemID)); target_item != nullptr){//Menu target
		unsigned int states = 0;
		dispatcher->dispatch_(*target_item, info, false, &states);

		if ((states & event::object::state_type::propagation_stopped) == 0u){//Propagation not stopped
			for (auto ancestor = target_item->get_first_ancestor_of_<menu::object, ui::window_surface>(); ancestor != nullptr; ancestor = ancestor->get_first_ancestor_of_<menu::object, ui::window_surface>()){
				dispatcher->dispatch_(*target_item, *ancestor, info, false, &states);
				if ((states & event::object::state_type::propagation_stopped) != 0u)
					break;//Propagation stopped
			}
		}

		if ((states & event::object::state_type::propagation_stopped) == 0u)//Propagation not stopped
			dispatcher->dispatch_(*target_item, target, info, false, &states);

		if ((states & event::object::state_type::default_prevented) == 0u)//Default not prevented
			event::draw_item_dispatcher::draw_item_(*target_item, *pinfo, info.hwnd, nullptr);
	}
	else
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

	return TRUE;
}

LRESULT winp::thread::surface_manager::measure_item_(ui::surface &target, const MSG &info, bool prevent_default){
	auto pinfo = reinterpret_cast<MEASUREITEMSTRUCT *>(info.lParam);
	auto dispatcher = find_dispatcher_(info.message);

	if (info.wParam != 0){

	}
	else if (auto target_item = dynamic_cast<menu::item_component *>(find_item_(pinfo->itemID)); target_item != nullptr){//Menu target
		unsigned int states = 0;
		dispatcher->dispatch_(*target_item, info, false, &states);

		if ((states & event::object::state_type::propagation_stopped) == 0u){//Propagation not stopped
			for (auto ancestor = target_item->get_first_ancestor_of_<menu::object, ui::window_surface>(); ancestor != nullptr; ancestor = ancestor->get_first_ancestor_of_<menu::object, ui::window_surface>()){
				dispatcher->dispatch_(*target_item, *ancestor, info, false, &states);
				if ((states & event::object::state_type::propagation_stopped) != 0u)//Propagation stopped
					break;
			}
		}

		if ((states & event::object::state_type::propagation_stopped) == 0u)//Propagation not stopped
			dispatcher->dispatch_(*target_item, target, info, false, &states);

		if ((states & event::object::state_type::default_prevented) == 0u){//Default not prevented
			auto size = event::draw_item_dispatcher::measure_item_(*target_item, info.hwnd, nullptr, nullptr);
			{//Update size
				pinfo->itemWidth = size.cx;
				pinfo->itemHeight = size.cy;
			}
		}
	}
	else
		return find_dispatcher_(info.message)->dispatch_(target, info, !prevent_default, nullptr);

	return TRUE;
}

LRESULT winp::thread::surface_manager::close_frame_(ui::surface &target, const MSG &info, bool prevent_default){
	unsigned int states = 0;
	find_dispatcher_(info.message)->dispatch_(target, info, false, &states);

	if ((states & event::object::state_type::default_prevented) != 0u)
		return 0;//Default prevented

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::size_frame_(ui::surface &target, const MSG &info, bool prevent_default){
	unsigned int states = 0;
	find_dispatcher_(info.message)->dispatch_(target, info, false, &states);

	if ((states & event::object::state_type::default_prevented) != 0u){//Default prevented
		auto drag_rect = reinterpret_cast<RECT *>(info.lParam);
		drag_rect->right = drag_rect->left;
		drag_rect->bottom = drag_rect->top;
	}

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::sized_frame_(ui::surface &target, const MSG &info, bool prevent_default){
	if (auto parent = target.get_parent_(); parent != nullptr)
		parent->call_hook_(ui::hook::child_size_change_hook_code);

	for (auto child : target.children_)
		child->call_hook_(ui::hook::parent_size_change_hook_code);

	return find_dispatcher_(info.message)->dispatch_(target, info, false, nullptr);
}

LRESULT winp::thread::surface_manager::move_frame_(ui::surface &target, const MSG &info, bool prevent_default){
	unsigned int states = 0;
	find_dispatcher_(info.message)->dispatch_(target, info, false, &states);

	if ((states & event::object::state_type::default_prevented) != 0u)//Default prevented
		*reinterpret_cast<RECT *>(info.lParam) = target.get_absolute_dimension_();

	return (prevent_default ? 0 : CallWindowProcW(target.get_default_message_entry_(), info.hwnd, info.message, info.wParam, info.lParam));
}

LRESULT winp::thread::surface_manager::moved_frame_(ui::surface &target, const MSG &info, bool prevent_default){
	return find_dispatcher_(info.message)->dispatch_(target, info, false, nullptr);
}

void winp::thread::surface_manager::track_mouse_leave_(HWND target, UINT flags){
	if (target != nullptr){
		TRACKMOUSEEVENT info{ sizeof(TRACKMOUSEEVENT), (TME_LEAVE | flags), target, 0 };
		TrackMouseEvent(&info);
		mouse_info_.tracking_mouse = true;
	}
}

bool winp::thread::surface_manager::menu_item_id_is_reserved_(UINT id){
	if (HIWORD(id) == 1u)
		return true;//Reserved ID

	switch (id & 0xFFF0){
	case SC_CLOSE:
	case SC_CONTEXTHELP:
	case SC_DEFAULT:
	case SC_HOTKEY:
	case SC_HSCROLL:
	case SCF_ISSECURE:
	case SC_KEYMENU:
	case SC_MAXIMIZE:
	case SC_MINIMIZE:
	case SC_MONITORPOWER:
	case SC_MOUSEMENU:
	case SC_MOVE:
	case SC_NEXTWINDOW:
	case SC_PREVWINDOW:
	case SC_RESTORE:
	case SC_SCREENSAVE:
	case SC_SIZE:
	case SC_TASKLIST:
	case SC_VSCROLL:
		return true;//Reserved for system menu items
	default:
		break;
	}

	return false;
}

bool winp::thread::surface_manager::initialize_dispatchers_(){
	default_dispatcher_ = std::make_shared<message::dispatcher>();

	dispatchers_[WINP_WM_PARENT_CHANGING] = std::make_shared<message::tree_dispatcher>();
	dispatchers_[WINP_WM_INDEX_CHANGING] = dispatchers_[WINP_WM_PARENT_CHANGING];
	dispatchers_[WINP_WM_CHILD_INDEX_CHANGING] = dispatchers_[WINP_WM_PARENT_CHANGING];

	dispatchers_[WINP_WM_CHILD_INSERTING] = dispatchers_[WINP_WM_PARENT_CHANGING];
	dispatchers_[WINP_WM_CHILD_REMOVING] = dispatchers_[WINP_WM_PARENT_CHANGING];

	dispatchers_[WINP_WM_PARENT_CHANGED] = dispatchers_[WINP_WM_PARENT_CHANGING];
	dispatchers_[WINP_WM_INDEX_CHANGED] = dispatchers_[WINP_WM_PARENT_CHANGING];
	dispatchers_[WINP_WM_CHILD_INDEX_CHANGED] = dispatchers_[WINP_WM_PARENT_CHANGING];

	dispatchers_[WINP_WM_CHILD_INSERTED] = dispatchers_[WINP_WM_PARENT_CHANGING];
	dispatchers_[WINP_WM_CHILD_REMOVED] = dispatchers_[WINP_WM_PARENT_CHANGING];

	dispatchers_[WM_NCCREATE] = std::make_shared<message::create_destroy_dispatcher>();
	dispatchers_[WM_NCDESTROY] = dispatchers_[WM_NCCREATE];

	dispatchers_[WM_ERASEBKGND] = std::make_shared<message::draw_dispatcher>();
	dispatchers_[WM_PAINT] = dispatchers_[WM_ERASEBKGND];
	dispatchers_[WM_PRINTCLIENT] = dispatchers_[WM_ERASEBKGND];

	dispatchers_[WINP_WM_ERASE_BACKGROUND] = dispatchers_[WM_ERASEBKGND];
	dispatchers_[WINP_WM_ERASE_NON_CLIENT_BACKGROUND] = dispatchers_[WM_ERASEBKGND];
	dispatchers_[WINP_WM_PAINT] = dispatchers_[WM_ERASEBKGND];

	dispatchers_[WM_DRAWITEM] = std::make_shared<message::draw_item_dispatcher>();
	dispatchers_[WM_MEASUREITEM] = dispatchers_[WM_DRAWITEM];

	dispatchers_[WINP_WM_MOUSELEAVE] = std::make_shared<message::mouse_dispatcher>();
	dispatchers_[WINP_WM_MOUSEENTER] = dispatchers_[WINP_WM_MOUSELEAVE];

	dispatchers_[WINP_WM_MOUSEMOVE] = dispatchers_[WINP_WM_MOUSELEAVE];
	dispatchers_[WINP_WM_MOUSEWHEEL] = dispatchers_[WINP_WM_MOUSELEAVE];

	dispatchers_[WINP_WM_MOUSEDOWN] = dispatchers_[WINP_WM_MOUSELEAVE];
	dispatchers_[WINP_WM_MOUSEUP] = dispatchers_[WINP_WM_MOUSELEAVE];
	dispatchers_[WINP_WM_MOUSEDBLCLK] = dispatchers_[WINP_WM_MOUSELEAVE];

	dispatchers_[WINP_WM_MOUSEDRAG] = dispatchers_[WINP_WM_MOUSELEAVE];
	dispatchers_[WINP_WM_MOUSEDRAGBEGIN] = dispatchers_[WINP_WM_MOUSELEAVE];
	dispatchers_[WINP_WM_MOUSEDRAGEND] = dispatchers_[WINP_WM_MOUSELEAVE];

	dispatchers_[WM_SETCURSOR] = std::make_shared<message::cursor_dispatcher>();
	dispatchers_[WINP_WM_FOCUS] = std::make_shared<message::focus_dispatcher>();
	dispatchers_[WINP_WM_KEY] = std::make_shared<message::key_dispatcher>();

	dispatchers_[WM_UNINITMENUPOPUP] = std::make_shared<message::menu_dispatcher>();
	dispatchers_[WM_INITMENUPOPUP] = dispatchers_[WM_UNINITMENUPOPUP];
	dispatchers_[WINP_WM_MENU_INIT_ITEM] = dispatchers_[WM_UNINITMENUPOPUP];

	dispatchers_[WINP_WM_MENU_SELECT] = dispatchers_[WM_UNINITMENUPOPUP];
	dispatchers_[WINP_WM_MENU_CHECK] = dispatchers_[WM_UNINITMENUPOPUP];
	dispatchers_[WINP_WM_MENU_UNCHECK] = dispatchers_[WM_UNINITMENUPOPUP];

	dispatchers_[WINP_WM_CONTEXT_MENU_QUERY] = dispatchers_[WM_UNINITMENUPOPUP];
	dispatchers_[WINP_WM_CONTEXT_MENU_REQUEST] = dispatchers_[WM_UNINITMENUPOPUP];
	dispatchers_[WM_CONTEXTMENU] = dispatchers_[WM_UNINITMENUPOPUP];

	dispatchers_[WM_CLOSE] = std::make_shared<message::frame_dispatcher>();
	dispatchers_[WM_SIZING] = dispatchers_[WM_CLOSE];
	dispatchers_[WM_MOVING] = dispatchers_[WM_CLOSE];
	dispatchers_[WM_SIZE] = dispatchers_[WM_CLOSE];
	dispatchers_[WM_MOVE] = dispatchers_[WM_CLOSE];

	return true;
}

winp::message::dispatcher *winp::thread::surface_manager::find_dispatcher_(UINT msg){
	auto it = dispatchers_.find(msg);
	return ((it == dispatchers_.end()) ? default_dispatcher_.get() : it->second.get());
}

LRESULT CALLBACK winp::thread::surface_manager::entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam){
	auto &manager = app::object::this_thread.surface_manager_;

	auto object = manager.find_object_(handle);
	if (object == nullptr)//Forward message
		return ((IsWindowUnicode(handle) == FALSE) ? CallWindowProcA(DefWindowProcA, handle, msg, wparam, lparam) : CallWindowProcW(DefWindowProcW, handle, msg, wparam, lparam));

	MSG info{ handle, msg, wparam, lparam };
	auto io_surface = dynamic_cast<ui::window_surface *>(object);

	switch (msg){
	case WM_NCDESTROY:
		return manager.destroy_window_(*object, info);
	case WM_PAINT:
		return manager.draw_(*object, info, false, m_rect_type{});
	case WM_PRINTCLIENT:
		return manager.draw_(*object, info, false, object->get_client_dimension_());
	case WM_COMMAND:
		return manager.command_(*object, info, false);
	case WM_SYSCOMMAND:
		return manager.system_command_(*object, info, false);
	case WM_UNINITMENUPOPUP:
		return manager.menu_uninit_(*object, info, false);
	case WM_INITMENUPOPUP:
		return manager.menu_init_(*object, info, false);
	case WM_MENUCOMMAND:
		return manager.menu_select_(*object, info, false);
	case WM_DRAWITEM:
		return manager.draw_item_(*object, info, false);
	case WM_MEASUREITEM:
		return manager.measure_item_(*object, info, false);
	case WM_CLOSE:
		return manager.close_frame_(*object, info, false);
	case WM_SIZING:
		return manager.size_frame_(*object, info, false);
	case WM_SIZE:
		return manager.sized_frame_(*object, info, false);
	case WM_MOVING:
		return manager.move_frame_(*object, info, false);
	case WM_MOVE:
		return manager.moved_frame_(*object, info, false);
	default:
		break;
	}

	if (io_surface != nullptr){
		switch (msg){
		case WM_NCMOUSELEAVE:
			return manager.mouse_nc_leave_(*io_surface, info, GetMessagePos(), false);
		case WM_MOUSELEAVE:
			return manager.mouse_leave_(*io_surface, info, GetMessagePos(), false);
		case WM_NCMOUSEMOVE:
			return manager.mouse_nc_move_(*io_surface, info, GetMessagePos(), false);
		case WM_MOUSEMOVE:
			return manager.mouse_move_(*io_surface, info, GetMessagePos(), false);
		case WM_LBUTTONDOWN:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_MBUTTONDOWN:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_RBUTTONDOWN:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_LBUTTONUP:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_MBUTTONUP:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_RBUTTONUP:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_LBUTTONDBLCLK:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_MBUTTONDBLCLK:
			return manager.mouse_other_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_RBUTTONDBLCLK:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_NCLBUTTONDOWN:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_NCMBUTTONDOWN:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_NCRBUTTONDOWN:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_NCLBUTTONUP:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_NCMBUTTONUP:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_NCRBUTTONUP:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_NCLBUTTONDBLCLK:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_LBUTTON, false);
		case WM_NCMBUTTONDBLCLK:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_MBUTTON, false);
		case WM_NCRBUTTONDBLCLK:
			return manager.nc_mouse_other_(*io_surface, info, GetMessagePos(), MK_RBUTTON, false);
		case WM_SETCURSOR:
			return manager.set_cursor_(*io_surface, info, false);
		case WM_SETFOCUS:
			return manager.set_focus_(*io_surface, info, false);
		case WM_KILLFOCUS:
			return manager.kill_focus_(*io_surface, info, false);
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			return manager.key_(*io_surface, info, false);
		case WM_CONTEXTMENU:
			return manager.context_menu_(*io_surface, info, false);
		default:
			break;
		}
	}

	return manager.find_dispatcher_(msg)->dispatch_(*object,info, true, nullptr);
}

LRESULT CALLBACK winp::thread::surface_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
	switch (code){
	case HCBT_CREATEWND:
		app::object::this_thread.surface_manager_.create_window_(reinterpret_cast<HWND>(wparam), *reinterpret_cast<CBT_CREATEWNDW *>(lparam));
		break;
	default:
		break;
	}

	return CallNextHookEx(nullptr, code, wparam, lparam);
}

std::shared_ptr<winp::message::dispatcher> winp::thread::surface_manager::default_dispatcher_;

std::unordered_map<UINT, std::shared_ptr<winp::message::dispatcher>> winp::thread::surface_manager::dispatchers_;

bool winp::thread::surface_manager::unused_ = initialize_dispatchers_();
