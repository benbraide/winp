#include "../app/app_object.h"

winp::menu::object::object() = default;

winp::menu::object::object(thread::object &thread)
	: group(thread){}

winp::menu::object::object(menu::item &parent)
	: group(parent.get_thread()){
	change_parent_(&parent);
}

winp::menu::object::object(ui::window_surface &parent)
	: group(parent.get_thread()){
	change_parent_(&parent);
}

winp::menu::object::~object(){
	destruct();
}

bool winp::menu::object::is_popup(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_popup_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_popup_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_popup_(); }, thread::queue::send_priority, id_);
}

void winp::menu::object::event_handlers_count_changed_(event::manager_base &e, std::size_t previous_count, std::size_t current_count){
	if (&e == &draw_item_event && !children_.empty() && (previous_count == 0u || current_count == 0u))
		update_children_types_();
}

bool winp::menu::object::create_(){
	if (get_handle_() != nullptr)
		return true;

	auto parent = get_parent_();
	auto window_parent = dynamic_cast<ui::window_surface *>(parent);
	auto item_parent = ((window_parent == nullptr) ? dynamic_cast<menu::item *>(parent) : nullptr);

	if (window_parent == nullptr)
		set_handle_(CreatePopupMenu());
	else//Associated with a window object
		set_handle_(CreateMenu());

	auto handle = get_handle_();
	if (handle == nullptr)
		return false;

	MENUINFO info{
		sizeof(MENUINFO),
		MIM_STYLE,
		MNS_NOTIFYBYPOS
	};
	SetMenuInfo(static_cast<HMENU>(handle), &info);

	thread_.surface_manager_.map_[handle] = this;
	dispatch_message_(WM_NCCREATE, 0, 0);

	for (auto child : children_)//Create children
		child->create_();

	auto parent_handle = ((window_parent == nullptr) ? nullptr : window_parent->get_handle_());
	if (parent_handle != nullptr){
		SetMenu(static_cast<HWND>(parent_handle), static_cast<HMENU>(handle));
		DrawMenuBar(static_cast<HWND>(parent_handle));
	}
	else if (item_parent != nullptr)
		item_parent->update_popup_();

	return true;
}

bool winp::menu::object::destroy_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return true;

	if (DestroyMenu(static_cast<HMENU>(handle)) == FALSE)
		return false;

	set_handle_(nullptr);
	dispatch_message_(WM_NCDESTROY, 0, 0);

	if (!thread_.surface_manager_.map_.empty())
		thread_.surface_manager_.map_.erase(handle);

	auto parent = get_parent_();
	auto window_parent = dynamic_cast<ui::window_surface *>(parent);
	auto item_parent = ((window_parent == nullptr) ? dynamic_cast<menu::item *>(parent) : nullptr);

	auto parent_handle = ((window_parent == nullptr) ? nullptr : window_parent->get_handle_());
	if (parent_handle != nullptr)//Remove association
		SetMenu(static_cast<HWND>(parent_handle), nullptr);
	else if (item_parent != nullptr)
		item_parent->update_popup_();

	return true;
}

HANDLE winp::menu::object::get_handle_() const{
	return ui::object::get_handle_();
}

LRESULT winp::menu::object::dispatch_message_(UINT msg, WPARAM wparam, LPARAM lparam, bool call_default){
	return find_dispatcher_(msg)->dispatch_(*this, MSG{ nullptr, msg, wparam, lparam }, call_default, nullptr);
}

UINT winp::menu::object::get_types_(std::size_t index) const{
	return ((draw_item_event.count_() == 0u && dynamic_cast<const event::draw_item_handler *>(this) == nullptr) ? 0u : MFT_OWNERDRAW);
}

std::size_t winp::menu::object::get_count_() const{
	return 0u;
}

bool winp::menu::object::handle_parent_change_event_(event::tree &e){
	auto prent = e.get_attached_parent();
	return (prent == nullptr || dynamic_cast<menu::item *>(prent) != nullptr || dynamic_cast<ui::window_surface *>(prent) != nullptr);
}

void winp::menu::object::handle_parent_changed_event_(event::tree &e){
	auto previous_parent = e.get_attached_parent();
	if (dynamic_cast<ui::window_surface *>(get_parent_()) == dynamic_cast<ui::window_surface *>(previous_parent)){
		auto window_previous_parent = dynamic_cast<ui::window_surface *>(previous_parent);
		if (window_previous_parent != nullptr)
			SetMenu(static_cast<HWND>(window_previous_parent->get_handle_()), nullptr);

		auto window_parent = dynamic_cast<ui::window_surface *>(get_parent_());
		if (window_parent != nullptr)
			SetMenu(static_cast<HWND>(window_parent->get_handle_()), static_cast<HMENU>(get_handle_()));
	}
	else{//Recreate
		destroy_();
		create_();
	}
}

void winp::menu::object::handle_index_changed_event_(event::tree &e){}

std::size_t winp::menu::object::get_absolute_index_() const{
	return 0u;
}

bool winp::menu::object::handle_found_in_surface_manager_(HANDLE value) const{
	return (thread_.surface_manager_.map_.find(value) != thread_.surface_manager_.map_.end());
}

void winp::menu::object::update_surface_manager_(bool add){
	auto handle = get_handle_();
	if (handle == nullptr)
		return;

	if (add)
		thread_.surface_manager_.map_[handle] = this;
	else if (!thread_.surface_manager_.map_.empty())
		thread_.surface_manager_.map_.erase(handle);
}

void winp::menu::object::redraw_(){
	if (get_handle_() != nullptr)
		return;

	auto window_parent = dynamic_cast<ui::window_surface *>(get_parent_());
	auto parent_handle = ((window_parent == nullptr) ? nullptr : window_parent->get_handle_());

	if (parent_handle != nullptr)
		DrawMenuBar(static_cast<HWND>(parent_handle));
}

bool winp::menu::object::is_popup_() const{
	return (dynamic_cast<ui::window_surface *>(get_parent_()) == nullptr);
}
