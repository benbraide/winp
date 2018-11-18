#include "../app/app_object.h"
#include "message_dispatcher.h"

winp::message::dispatcher::dispatcher()
	: event_dispatcher_(std::make_shared<event::dispatcher>()){}

winp::message::dispatcher::dispatcher(bool){}

void winp::message::dispatcher::cleanup_(){}

LRESULT winp::message::dispatcher::dispatch_(ui::object &target, const MSG &info, bool call_default, bool is_post){
	auto e = create_event_(target, info, call_default);
	if (e == nullptr)//Error
		return 0;

	pre_dispatch_(*e, call_default);
	if (!e->default_prevented_()){
		do_dispatch_(*e, call_default);
		post_dispatch_(*e);
	}

	if (!is_post)
		cleanup_();

	return e->get_result_();
}

void winp::message::dispatcher::pre_dispatch_(event::object &e, bool &call_default){}

void winp::message::dispatcher::post_dispatch_(event::object &e){}

void winp::message::dispatcher::do_dispatch_(event::object &e, bool call_default){
	fire_event_(e);
	if (!e.default_prevented_())
		do_default_(e, call_default);
}

void winp::message::dispatcher::do_default_(event::object &e, bool call_default){
	if (!is_doing_default_){//Call dispatch 
		is_doing_default_ = true;
		event_dispatcher_->dispatch_(e);
	}

	if (!is_doing_default_ || !call_default || e.default_prevented_()){//Prevent system handling
		is_doing_default_ = false;
		return;
	}

	e.set_result_(call_default_(e), false);
	is_doing_default_ = false;
}

LRESULT winp::message::dispatcher::call_default_(event::object &e){
	auto &info = *e.get_info();
	return CallWindowProcW(get_default_message_entry_of_(*e.get_context()), info.hwnd, info.message, info.wParam, info.lParam);
}

void winp::message::dispatcher::fire_event_(event::object &e){}

std::shared_ptr<winp::event::object> winp::message::dispatcher::create_event_(ui::object &target, const MSG &info, bool call_default){
	return create_new_event_<event::object>(target, info, call_default);
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
	auto msg = e.get_info()->message;
	if (msg != WINP_WM_PARENT_CHANGED && msg != WINP_WM_INDEX_CHANGED){
		auto tree_target = dynamic_cast<ui::tree *>(e.get_context());
		if (tree_target == nullptr)
			return;

		switch (msg){
		case WINP_WM_CHILD_INDEX_CHANGED:
			fire_event_of_(*tree_target, tree_target->child_index_change_event, e);
			break;
		case WINP_WM_CHILD_INSERTED:
			fire_event_of_(*tree_target, tree_target->child_insert_event, e);
			break;
		case WINP_WM_CHILD_REMOVED:
			fire_event_of_(*tree_target, tree_target->child_remove_event, e);
			break;
		default:
			break;
		}
	}
	else if (msg != WINP_WM_PARENT_CHANGED)
		fire_event_of_(*e.get_context(), e.get_context()->parent_change_event, e);
	else if (msg != WINP_WM_INDEX_CHANGED)
		fire_event_of_(*e.get_context(), e.get_context()->index_change_event, e);
}

std::shared_ptr<winp::event::object> winp::message::tree_dispatcher::create_event_(ui::object &target, const MSG &info, bool call_default){
	return create_new_event_<event::tree>(target, info, call_default);
}

winp::message::create_destroy_dispatcher::create_destroy_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::create_destroy_dispatcher>();
}

void winp::message::create_destroy_dispatcher::fire_event_(event::object &e){
	auto window_target = dynamic_cast<ui::window_surface *>(e.get_context());
	if (window_target == nullptr)
		return;//Window target is required

	if (e.get_info()->message == WM_CREATE)
		fire_event_of_(*window_target, window_target->create_event, e);
	else
		fire_event_of_(*window_target, window_target->destroy_event, e);
}

winp::message::draw_dispatcher::draw_dispatcher()
	: dispatcher(false){
	event_dispatcher_ = std::make_shared<event::draw_dispatcher>();
}

void winp::message::draw_dispatcher::cleanup_(){
	e_ = nullptr;
}

void winp::message::draw_dispatcher::post_dispatch_(event::object &e){
	ui::surface *child_target;
	auto saved_offset = offset_;
	auto saved_result = get_result_of_(e);

	auto &info = *e.get_info();
	for (auto child : get_children_of_(*dynamic_cast<ui::surface *>(e.get_context()))){
		if ((child_target = dynamic_cast<ui::surface *>(child)) != nullptr && dynamic_cast<ui::window_surface *>(child) == nullptr)
			dispatch_(*child_target, info, false, true);

		offset_ = saved_offset;//Restore saved offset
		set_result_of_(e, saved_result, true);//Restore result
	}
}

void winp::message::draw_dispatcher::fire_event_(event::object &e){
	auto visible_target = dynamic_cast<ui::visible_surface *>(e.get_context());
	if (visible_target == nullptr)
		return;//Visible target required

	e_->set_target_(visible_target, offset_);
	if (e_->get_info()->message == WM_ERASEBKGND)
		fire_event_of_(*visible_target, visible_target->background_erase_event, *e_);
	else
		fire_event_of_(*visible_target, visible_target->draw_event, *e_);
}

std::shared_ptr<winp::event::object> winp::message::draw_dispatcher::create_event_(ui::object &target, const MSG &info, bool call_default){
	if (e_ == nullptr){//Initialize
		e_ = create_new_event_<event::draw>(target, info, call_default);
		offset_ = POINT{};
	}

	return e_;
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

std::shared_ptr<winp::event::object> winp::message::cursor_dispatcher::create_event_(ui::object &target, const MSG &info, bool call_default){
	return create_new_event_<event::cursor>(target, info, call_default);
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

void winp::message::mouse_dispatcher::cleanup_(){
	e_ = nullptr;
	ev_ = nullptr;
}

void winp::message::mouse_dispatcher::post_dispatch_(event::object &e){
	if (e_->bubble_to_type_<ui::io_surface>()){
		auto &info = *e.get_info();
		auto saved_result = get_result_of_(e);

		dispatch_(*dynamic_cast<ui::surface *>(e_->get_context()), info, false, true);
		set_result_of_(e, saved_result, true);//Restore result
	}
}

void winp::message::mouse_dispatcher::fire_event_(event::object &e){
	if (ev_ != nullptr)
		fire_event_of_(*dynamic_cast<ui::io_surface *>(e.get_context()), *ev_, *e_);
}

std::shared_ptr<winp::event::object> winp::message::mouse_dispatcher::create_event_(ui::object &target, const MSG &info, bool call_default){
	if (e_ == nullptr){//Initialize
		auto io_target = dynamic_cast<ui::io_surface *>(&target);
		if (io_target == nullptr && (io_target = get_first_ancestor_of_<ui::io_surface>(target)) == nullptr)
			return nullptr;//IO target required

		auto mouse_position = GetMessagePos();
		POINT computed_mouse_position{ GET_X_LPARAM(mouse_position), GET_Y_LPARAM(mouse_position) };

		resolve_(*io_target, info.message);
		e_ = create_new_event_<event::mouse>(target, info, call_default, computed_mouse_position, button_);
	}

	return e_;
}

void winp::message::mouse_dispatcher::resolve_(ui::io_surface &target, UINT msg){
	switch (msg){
	case WINP_WM_MOUSELEAVE:
		ev_ = &target.mouse_event.leave;
		button_ = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEENTER:
		ev_ = &target.mouse_event.enter;
		button_ = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEMOVE:
		ev_ = &target.mouse_event.move;
		button_ = event::mouse::button_type::nil;
		break;
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		ev_ = &target.mouse_event.wheel;
		button_ = event::mouse::button_type::nil;
		break;
	case WM_NCLBUTTONDOWN:
	case WM_LBUTTONDOWN:
		ev_ = &target.mouse_event.down;
		button_ = event::mouse::button_type::left;
		break;
	case WM_NCMBUTTONDOWN:
	case WM_MBUTTONDOWN:
		ev_ = &target.mouse_event.down;
		button_ = event::mouse::button_type::middle;
		break;
	case WM_NCRBUTTONDOWN:
	case WM_RBUTTONDOWN:
		ev_ = &target.mouse_event.down;
		button_ = event::mouse::button_type::right;
		break;
	case WM_NCLBUTTONUP:
	case WM_LBUTTONUP:
		ev_ = &target.mouse_event.up;
		button_ = event::mouse::button_type::left;
		break;
	case WM_NCMBUTTONUP:
	case WM_MBUTTONUP:
		ev_ = &target.mouse_event.up;
		button_ = event::mouse::button_type::middle;
		break;
	case WM_NCRBUTTONUP:
	case WM_RBUTTONUP:
		ev_ = &target.mouse_event.up;
		button_ = event::mouse::button_type::right;
		break;
	case WM_NCLBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		ev_ = &target.mouse_event.double_click;
		button_ = event::mouse::button_type::left;
		break;
	case WM_NCMBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		ev_ = &target.mouse_event.double_click;
		button_ = event::mouse::button_type::middle;
		break;
	case WM_NCRBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		ev_ = &target.mouse_event.double_click;
		button_ = event::mouse::button_type::right;
		break;
	case WINP_WM_MOUSEDRAG:
		ev_ = &target.mouse_event.drag;
		button_ = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEDRAGBEGIN:
		ev_ = &target.mouse_event.drag_begin;
		button_ = event::mouse::button_type::nil;
		break;
	case WINP_WM_MOUSEDRAGEND:
		ev_ = &target.mouse_event.drag_end;
		button_ = event::mouse::button_type::nil;
		break;
	default:
		ev_ = nullptr;
		button_ = event::mouse::button_type::nil;
		break;
	}
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

void winp::message::key_dispatcher::cleanup_(){
	e_ = nullptr;
	ev_ = nullptr;
}

void winp::message::key_dispatcher::post_dispatch_(event::object &e){
	if (e_->bubble_to_type_<ui::io_surface>()){
		auto &info = *e.get_info();
		auto saved_result = get_result_of_(e);

		dispatch_(*dynamic_cast<ui::surface *>(e_->get_context()), info, false, true);
		set_result_of_(e, saved_result, true);//Restore result
	}
}

void winp::message::key_dispatcher::fire_event_(event::object &e){
	if (ev_ != nullptr)
		fire_event_of_(*dynamic_cast<ui::io_surface *>(e.get_context()), *ev_, *e_);
}

std::shared_ptr<winp::event::object> winp::message::key_dispatcher::create_event_(ui::object &target, const MSG &info, bool call_default){
	if (e_ == nullptr){//Initialize
		auto io_target = dynamic_cast<ui::io_surface *>(&target);
		if (io_target == nullptr && (io_target = get_first_ancestor_of_<ui::io_surface>(target)) == nullptr)
			return nullptr;//IO target required

		resolve_(*io_target, info.message);
		e_ = create_new_event_<event::key>(target, info, call_default);
	}

	return e_;
}

void winp::message::key_dispatcher::resolve_(ui::io_surface &target, UINT msg){
	switch (msg){
	case WM_KEYDOWN:
		ev_ = &target.key_event.down;
		break;
	case WM_KEYUP:
		ev_ = &target.key_event.up;
		break;
	case WM_CHAR:
		ev_ = &target.key_event.press;
		break;
	default:
		break;
	}
}
