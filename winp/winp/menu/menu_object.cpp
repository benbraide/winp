#include "../app/app_object.h"

winp::menu::object::object() = default;

winp::menu::object::object(thread::object &thread)
	: io_surface(thread), window_(thread){}

winp::menu::object::object(menu::item &parent)
	: io_surface(parent.get_thread()), window_(parent.get_thread()){
	set_parent_(&parent);
}

winp::menu::object::object(ui::window_surface &parent)
	: io_surface(parent.get_thread()), window_(parent.get_thread()){
	set_parent_(&parent);
}

winp::menu::object::~object(){
	destruct_();
}

std::size_t winp::menu::object::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_index_()); }, thread::queue::send_priority, id_);
		return static_cast<std::size_t>(-1);
	}

	return thread_.queue.add([this]{ return get_absolute_index_(); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::object::is_popup(const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_popup_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([this]{ return is_popup_(); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::object::create_(){
	if (get_handle_() != nullptr)
		return true;

	auto window_parent = dynamic_cast<ui::window_surface *>(get_parent_());
	if (window_parent == nullptr)
		set_handle_(CreatePopupMenu());
	else//Associated with a window object
		set_handle_(CreateMenu());

	auto handle = get_handle_();
	if (handle == nullptr)
		return false;

	auto parent_handle = ((window_parent == nullptr) ? nullptr : window_parent->get_handle_());
	if (parent_handle != nullptr){
		SetMenu(static_cast<HWND>(parent_handle), static_cast<HMENU>(handle));
		DrawMenuBar(static_cast<HWND>(parent_handle));
	}

	window_.class_name_ = L"#32768";
	window_.parent_ = this;

	dispatch_message_(WM_CREATE, 0, 0);
	for (auto child : children_)//Create children
		child->create_();

	return true;
}

bool winp::menu::object::destroy_(){
	auto handle = get_handle_();
	if (handle == nullptr)
		return true;

	if (DestroyMenu(static_cast<HMENU>(handle)) == FALSE)
		return false;

	set_handle_(nullptr);
	dispatch_message_(WM_DESTROY, 0, 0);

	return true;
}

bool winp::menu::object::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return (io_surface::validate_parent_change_ (value, index) && (dynamic_cast<menu::item *>(value) != nullptr || dynamic_cast<ui::window_surface *>(value) != nullptr));
}

void winp::menu::object::parent_changed_(ui::tree *previous_parent, std::size_t previous_index){
	if (get_handle_() != nullptr && dynamic_cast<ui::window_surface *>(get_parent_()) != dynamic_cast<ui::window_surface *>(previous_parent)){
		destroy_();
		create_();
	}

	io_surface::parent_changed_(previous_parent, previous_index);
}

LRESULT winp::menu::object::dispatch_message_(UINT msg, WPARAM wparam, LPARAM lparam, bool call_default){
	return find_dispatcher_(msg)->dispatch_(*this, MSG{ nullptr, msg, wparam, lparam }, call_default);
}

UINT winp::menu::object::get_types_(std::size_t index) const{
	return 0u;
}

UINT winp::menu::object::get_states_(std::size_t index) const{
	return 0u;
}

std::size_t winp::menu::object::get_absolute_index_of_(const menu::component &child) const{
	return find_child_(dynamic_cast<const ui::object &>(child));
}

std::size_t winp::menu::object::get_count_() const{
	return 0u;
}

std::size_t winp::menu::object::get_absolute_index_() const{
	return 0u;
}

void winp::menu::object::destruct_(){
	thread_.queue.add([=]{
		destroy_();
	}, thread::queue::send_priority, id_).get();
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
