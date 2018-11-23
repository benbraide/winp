#include "menu_wrapper.h"

winp::menu::wrapper::wrapper() = default;

winp::menu::wrapper::wrapper(thread::object &thread)
	: object(thread){}

winp::menu::wrapper::wrapper(HMENU value){
	init(value);
}

winp::menu::wrapper::wrapper(thread::object &thread, HMENU value)
	: object(thread){
	init(value);
}

winp::menu::wrapper::~wrapper(){
	destruct();
}

bool winp::menu::wrapper::init(HMENU value, const std::function<void(wrapper &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = init_(value);
		if (callback)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = init_(value);
		if (callback)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::menu::wrapper::create_(){
	return true;
}

bool winp::menu::wrapper::destroy_(){
	if (get_handle_() == nullptr)
		return true;

	update_surface_manager_(false);
	set_handle_(nullptr);

	auto children_copy = children_;
	for (auto child : children_copy)
		child->destruct();

	return true;
}

bool winp::menu::wrapper::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return false;
}

void winp::menu::wrapper::child_removed_(ui::object &child, std::size_t previous_index){
	object::child_removed_(child, previous_index);
	if (!item_list_.empty())
		item_list_.erase(dynamic_cast<menu::component *>(&child));
}

bool winp::menu::wrapper::init_(HMENU value){
	if (value != nullptr && handle_found_in_surface_manager_(value))
		return false;//Cannot wrap a managed menu

	destroy_();
	auto result = wrap_(value);

	set_handle_(value);
	update_surface_manager_(true);

	return result;
}

bool winp::menu::wrapper::wrap_(HMENU value){
	if (value == nullptr)
		return true;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_SUBMENU)
	};

	MENUITEMINFOW temp_info{
		sizeof(MENUITEMINFOW)
	};

	item_ptr_type item;
	menu::item *menu_item = nullptr;
	std::wstring label;

	auto count = GetMenuItemCount(value);
	for (auto index = 0; index < count; ++index){
		if (GetMenuItemInfoW(value, index, TRUE, &info) == FALSE)
			continue;

		if ((info.fType & MFT_SEPARATOR) == 0u){
			menu_item = dynamic_cast<menu::item *>((item = ((info.hSubMenu == nullptr) ? std::make_shared<menu::item>(*this) : std::make_shared<menu::link>(*this))).get());
			if (info.wID == 0u){//Update ID
				temp_info.fMask = MIIM_ID;
				temp_info.wID = menu_item->local_id_;
				SetMenuItemInfoW(value, index, TRUE, &temp_info);
			}
			else//Set ID
				menu_item->local_id_ = static_cast<WORD>(info.wID);

			if (info.cch > 0u){//Cache text
				label.resize(info.cch);

				temp_info.fMask = MIIM_STRING;
				temp_info.dwTypeData = label.data();
				temp_info.cch = (info.cch + 1u);

				if (GetMenuItemInfoW(value, index, TRUE, &temp_info) != FALSE)
					menu_item->set_label_(label);
			}

			menu_item->is_created_ = true;
			menu_item->states_ = info.fState;
			menu_item->types_ = info.fType;

			menu_item->bitmap_ = info.hbmpItem;
			menu_item->checked_bitmap_ = info.hbmpChecked;
			menu_item->unchecked_bitmap_ = info.hbmpUnchecked;

			if (info.hSubMenu != nullptr){//Create sub menu
				dynamic_cast<menu::link *>(menu_item)->create_popup_<wrapper>([=](wrapper &target){
					return true;
				}, info.hSubMenu);
			}
		}
		else//Separator
			item = std::make_shared<menu::separator>(*this);

		item_list_[item.get()] = item;
	}

	return true;
}
