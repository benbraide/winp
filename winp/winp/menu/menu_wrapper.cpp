#include "menu_wrapper.h"

winp::menu::wrapper::wrapper() = default;

winp::menu::wrapper::wrapper(thread::object &thread)
	: object(thread){}

winp::menu::wrapper::wrapper(HMENU value, menu::item *parent){
	init(value, parent);
}

winp::menu::wrapper::wrapper(thread::object &thread, HMENU value, menu::item *parent)
	: object(thread){
	init(value, parent);
}

winp::menu::wrapper::~wrapper(){
	destruct_();
}

void winp::menu::wrapper::init(HMENU value, menu::item *parent){
	thread_.queue.post([=]{
		init_(value, parent);
	}, thread::queue::send_priority, id_);
}

bool winp::menu::wrapper::create_(){
	return true;
}

bool winp::menu::wrapper::destroy_(){
	set_handle_(nullptr);

	if (!item_list_.empty())
		item_list_.clear();

	if (!children_.empty())
		children_.clear();

	return true;
}

bool winp::menu::wrapper::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return false;
}

void winp::menu::wrapper::child_removed_(ui::object &child, std::size_t previous_index){
	for (auto it = item_list_.begin(); it != item_list_.end(); ++it){
		if (dynamic_cast<ui::object *>(it->get()) == &child){
			item_list_.erase(it);
			break;
		}
	}

	object::child_removed_(child, previous_index);
}

void winp::menu::wrapper::init_(HMENU value, menu::item *parent){
	if (parent != nullptr)
		change_parent_(parent);

	wrap_(value);
	set_handle_(value);
}

void winp::menu::wrapper::wrap_(HMENU value){
	if (value == nullptr)
		return;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_SUBMENU)
	};

	MENUITEMINFOW temp_info{
		sizeof(MENUITEMINFOW)
	};

	item_ptr_type item;
	menu::link *menu_item = nullptr;
	std::wstring label;

	auto count = GetMenuItemCount(value);
	for (auto index = 0; index < count; ++index){
		if (GetMenuItemInfoW(value, index, TRUE, &info) == FALSE)
			continue;

		if ((info.fType & MFT_SEPARATOR) == 0u){
			menu_item = dynamic_cast<menu::link *>((item = std::make_shared<menu::link>(*this, false)).get());
			if (info.wID == 0u){//Generate ID and update
				menu_item->generate_id_();
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

			menu_item->states_ = info.fState;
			menu_item->types_ = info.fType;

			menu_item->bitmap_ = info.hbmpItem;
			menu_item->checked_bitmap_ = info.hbmpChecked;
			menu_item->unchecked_bitmap_ = info.hbmpUnchecked;

			if (info.hSubMenu != nullptr){//Create sub menu
				menu_item->set_target_<wrapper>([=](wrapper &target){
					return true;
				}, info.hSubMenu);
			}
		}
		else//Separator
			item = std::make_shared<menu::item>(*this);

		item_list_.push_back(item);
	}
}
