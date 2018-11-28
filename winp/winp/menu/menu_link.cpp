#include "menu_link.h"

winp::menu::link::link() = default;

winp::menu::link::link(bool)
	: item(false){}

winp::menu::link::link(thread::object &thread)
	: item(thread){}

winp::menu::link::link(thread::object &thread, bool)
	: item(thread, false){}

winp::menu::link::link(ui::tree &parent)
	: item(parent){}

winp::menu::link::link(ui::tree &parent, bool)
	: item(parent){}

winp::menu::link::~link() = default;

void winp::menu::link::handle_child_inserted_event_(event::tree &e){
	if (e.get_target() != target_ptr_.get())
		target_ptr_ = nullptr;
	item::handle_child_inserted_event_(e);
}
