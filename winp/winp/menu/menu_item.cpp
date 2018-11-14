#include "../app/app_object.h"
#include "menu_item.h"

winp::menu::item::item() = default;

winp::menu::item::item(thread::object &thread)
	: object(thread){}

winp::menu::item::item(ui::tree &parent)
	: object(parent.get_thread()){
	set_parent_(&parent);
}

winp::menu::item::~item(){
	remove_parent_();
}

bool winp::menu::item::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return (object::validate_parent_change_(value, index));
}

void winp::menu::item::parent_changed_(ui::tree *previous_parent, std::size_t previous_index){
	if (previous_parent != nullptr){

	}

	object::parent_changed_(previous_parent, previous_index);
}

void winp::menu::item::index_changed_(ui::tree *previous_parent, std::size_t previous_index){
	object::index_changed_(previous_parent, previous_index);
}
