#include "../app/app_object.h"

winp::menu::separator::separator() = default;

winp::menu::separator::separator(thread::object &thread)
	: surface(thread){}

winp::menu::separator::separator(ui::tree &parent)
	: surface(parent.get_thread()){
	change_parent_(&parent);
}

winp::menu::separator::~separator(){
	destruct_();
}

std::size_t winp::menu::separator::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_index_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([this]{ return get_absolute_index_(); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::separator::create_(){
	if (is_created_)
		return true;

	auto parent = get_parent_();
	if (parent == nullptr)
		return false;

	auto handle = static_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr || IsMenu(handle) == FALSE)//Parent not created
		return true;

	UINT mask = (MIIM_FTYPE | MIIM_STATE | MIIM_DATA);
	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		mask,
		(MFT_SEPARATOR | dynamic_cast<menu::tree *>(parent)->get_types_(get_index_())),
		(MFS_GRAYED | dynamic_cast<menu::tree *>(parent)->get_states_(get_index_())),
		0,
		nullptr,
		nullptr,
		nullptr,
		reinterpret_cast<ULONG_PTR>(this)
	};

	if (InsertMenuItemW(handle, static_cast<UINT>(get_absolute_index_()), TRUE, &info) == FALSE)
		return false;

	auto menu_parent = dynamic_cast<menu::object *>(parent);
	if (menu_parent != nullptr)
		menu_parent->redraw_();

	is_created_ = true;
	return true;
}

bool winp::menu::separator::destroy_(){
	if (!is_created_)
		return true;

	is_created_ = false;
	auto parent = get_parent_();

	return ((parent == nullptr) ? true : remove_from_parent_(*parent));
}

bool winp::menu::separator::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return (surface::validate_parent_change_(value, index) && (value == nullptr || dynamic_cast<menu::tree *>(value) != nullptr));
}

void winp::menu::separator::parent_changed_(ui::tree *previous_parent, std::size_t previous_index){
	if (is_created_){//Move to new parent
		if (previous_parent != nullptr)//Remove from previous parent
			remove_from_parent_(*previous_parent);

		is_created_ = false;
		create_();
	}

	surface::parent_changed_(previous_parent, previous_index);
}

void winp::menu::separator::index_changed_(ui::tree *previous_parent, std::size_t previous_index){
	if (is_created_){//Adjust index
		auto parent = get_parent_();
		if (previous_parent == parent && parent != nullptr && remove_from_parent_(*parent)){
			is_created_ = false;
			create_();
		}
	}

	surface::index_changed_(previous_parent, previous_index);
}

bool winp::menu::separator::validate_child_insert_(const ui::object &child, std::size_t index) const{
	return false;
}

bool winp::menu::separator::validate_child_remove_(const ui::object &child) const{
	return false;
}

std::size_t winp::menu::separator::get_count_() const{
	return 1u;
}

std::size_t winp::menu::separator::get_absolute_index_() const{
	auto parent = dynamic_cast<menu::tree *>(get_parent_());
	return ((parent == nullptr) ? get_index_() : parent->get_absolute_index_of_(*this));
}

bool winp::menu::separator::remove_from_parent_(ui::tree &parent){
	auto handle = static_cast<HMENU>(parent.get_handle_());
	if (handle == nullptr || IsMenu(handle) == FALSE)//Parent not created
		return true;

	auto result = RemoveMenu(handle, static_cast<UINT>(get_absolute_index_()), MF_BYPOSITION);
	auto menu_parent = dynamic_cast<menu::object *>(&parent);
	if (menu_parent != nullptr)
		menu_parent->redraw_();

	return (result != FALSE);
}

bool winp::menu::separator::update_(const MENUITEMINFOW &info){
	auto parent = get_parent_();
	if (parent == nullptr)
		return true;

	auto handle = static_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr)//Parent not created
		return true;

	return (SetMenuItemInfoW(handle, static_cast<UINT>(get_absolute_index_()), TRUE, &info) != FALSE);
}

bool winp::menu::separator::update_states_(){
	auto parent = get_parent_();
	if (parent == nullptr)
		return false;

	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_STATE,												//Flags
		0,														//Types
		(MFS_GRAYED | dynamic_cast<menu::tree *>(parent)->get_states_(get_index_()))
	});
}

bool winp::menu::separator::update_types_(){
	auto parent = get_parent_();
	if (parent == nullptr)
		return false;

	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_FTYPE,												//Flags
		(MFT_SEPARATOR | dynamic_cast<menu::tree *>(parent)->get_types_(get_index_()))
	});
}

void winp::menu::separator::destruct_(){
	thread_.queue.add([=]{
		destroy_();
	}, thread::queue::send_priority, id_).get();
}
