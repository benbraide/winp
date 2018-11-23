#include "menu_link.h"

winp::menu::link::link() = default;

winp::menu::link::link(thread::object &thread)
	: item(thread){}

winp::menu::link::link(ui::tree &parent)
	: item(parent){}

winp::menu::link::~link() = default;

void winp::menu::link::child_removed_(ui::object &child, std::size_t previous_index){
	target_ptr_ = nullptr;
	item::child_removed_(child, previous_index);
}
