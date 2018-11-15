#include "../app/app_object.h"
#include "menu_item.h"

winp::menu::item::item(){
	local_id_ = thread_.menu_random_generator_(static_cast<WORD>(1), std::numeric_limits<WORD>::max());
}

winp::menu::item::item(thread::object &thread)
	: object(thread){
	local_id_ = thread_.menu_random_generator_(static_cast<WORD>(1), std::numeric_limits<WORD>::max());
}

winp::menu::item::item(ui::tree &parent)
	: object(parent.get_thread()){
	local_id_ = thread_.menu_random_generator_(static_cast<WORD>(1), std::numeric_limits<WORD>::max());
	set_parent_(&parent);
}

winp::menu::item::~item(){
	thread_.queue.add([=]{
		remove_parent_();
	}, thread::queue::send_priority, id_).get();
}

std::size_t winp::menu::item::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_index_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([this]{ return get_absolute_index_(); }, thread::queue::send_priority, id_).get();
}

WORD winp::menu::item::get_local_id() const{
	return local_id_;
}

bool winp::menu::item::destroy_(){
	return remove_parent_();
}

bool winp::menu::item::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return (object::validate_parent_change_(value, index) && (value == nullptr || dynamic_cast<tree *>(value) != nullptr));
}

void winp::menu::item::parent_changed_(ui::tree *previous_parent, std::size_t previous_index){
	if (previous_parent != nullptr)//Remove from previous parent
		remove_from_parent_(*previous_parent);

	auto parent = get_parent_();
	if (parent != nullptr)//Insert into new parent
		insert_into_parent_(*parent);

	object::parent_changed_(previous_parent, previous_index);
}

void winp::menu::item::index_changed_(ui::tree *previous_parent, std::size_t previous_index){
	auto parent = get_parent_();
	if (previous_parent == parent && parent != nullptr && remove_from_parent_(*parent))
		insert_into_parent_(*parent);

	object::index_changed_(previous_parent, previous_index);
}

std::size_t winp::menu::item::get_count_() const{
	return 1u;
}

std::size_t winp::menu::item::get_absolute_index_() const{
	auto parent = dynamic_cast<tree *>(get_parent_());
	return ((parent == nullptr) ? get_index_() : parent->get_absolute_index_of_(*this));
}

bool winp::menu::item::remove_from_parent_(ui::tree &parent){
	auto result = RemoveMenu(reinterpret_cast<HMENU>(parent.get_handle_()), local_id_, MF_BYCOMMAND);
	/*auto bar_parent = dynamic_cast<bar *>(previous_parent);
	if (bar_parent != nullptr && bar_parent->owner != nullptr)
		DrawMenuBar(bar_parent->owner->handle);*/

	return (result != FALSE);
}

bool winp::menu::item::insert_into_parent_(ui::tree &parent){
	auto handle = reinterpret_cast<HMENU>(parent.get_handle_());
	if (handle == nullptr)//Parent not created
		return true;

	UINT mask = (MIIM_ID | MIIM_DATA);
	if (!label_.empty())//Label set
		mask |= MIIM_STRING;

	if (types_ != 0u)//Type set
		mask |= MIIM_FTYPE;

	if (states_ != 0u)//States set
		mask |= MIIM_STATE;

	/*if (bitmap_ != nullptr)//Bitmap set
		mask |= MIIM_BITMAP;

	if (checked_bitmap_ != nullptr || unchecked_bitmap_ != nullptr)
		mask |= MIIM_CHECKMARKS;//Bitmaps set*/

	std::wstring joined_label, *plabel;
	if (!label_.empty() && !shortcut_.empty()){
		joined_label = (label_ + L"\t" + shortcut_);
		plabel = &joined_label;
	}
	else//No shortcut
		plabel = &label_;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),									//Size
		mask,													//Flags
		types_,													//Types
		states_,												//States
		local_id_,												//Id
		nullptr,												//Sub-menu
		nullptr,												//Checked bitmap
		nullptr,												//Unchecked bitmap
		reinterpret_cast<ULONG_PTR>(this),						//Data
		plabel->data(),											//String
		static_cast<UINT>(plabel->size()),						//String size
		nullptr													//Item bitmap
	};

	if (InsertMenuItemW(handle, static_cast<UINT>(get_absolute_index_()), TRUE, &info) == FALSE)
		return false;

	/*auto bar_parent = dynamic_cast<bar *>(parent);
	if (bar_parent != nullptr && bar_parent->owner != nullptr)
		DrawMenuBar(bar_parent->owner->handle);*/

	return true;
}

bool winp::menu::item::update_(const MENUITEMINFOW &info){
	auto parent = get_parent_();
	if (parent == nullptr)
		return true;

	auto handle = reinterpret_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr)//Parent not created
		return true;

	return (SetMenuItemInfoW(handle, local_id_, FALSE, &info) != FALSE);
}
