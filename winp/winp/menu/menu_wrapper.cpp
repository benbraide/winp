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

bool winp::menu::wrapper::init(HMENU value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, init_(value));
	});
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

bool winp::menu::wrapper::handle_parent_change_event_(event::tree &e){
	auto prent = e.get_attached_parent();
	return (prent == nullptr || dynamic_cast<menu::item *>(prent) != nullptr);
}

void winp::menu::wrapper::handle_child_inserted_event_(event::tree &e){
	if (!marked_items_.empty())
		marked_items_.clear();
}

void winp::menu::wrapper::handle_child_removed_event_(event::tree &e){
	if (!item_map_.empty()){
		if (auto it = item_map_.find(dynamic_cast<menu::component *>(e.get_target())); it != item_map_.end()){
			marked_items_.push_back(it->second);
			item_map_.erase(it);
		}
	}
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
			menu_item = dynamic_cast<menu::item *>((item = ((info.hSubMenu == nullptr) ? std::make_shared<menu::item>(*this, false) : std::make_shared<menu::link>(*this))).get());
			if (info.wID == 0u){//Generate and update ID
				menu_item->generate_id_();
				if (menu_item->local_id_ != 0u){
					temp_info.fMask = MIIM_ID;
					temp_info.wID = menu_item->local_id_;
					SetMenuItemInfoW(value, index, TRUE, &temp_info);
				}
			}
			else//Set ID
				menu_item->local_id_ = info.wID;

			menu_item->register_id_();
			if (info.cch > 0u){//Cache text
				label.resize(info.cch);

				temp_info.fMask = MIIM_STRING;
				temp_info.dwTypeData = label.data();
				temp_info.cch = (info.cch + 1u);

				if (GetMenuItemInfoW(value, index, TRUE, &temp_info) != FALSE)
					menu_item->set_label_(label);
			}

			menu_item->is_created_state_ = true;
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

		item_map_[item.get()] = item;
	}

	return true;
}
