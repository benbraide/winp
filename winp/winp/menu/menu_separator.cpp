#include "../app/app_object.h"

winp::menu::separator::separator() = default;

winp::menu::separator::separator(thread::object &thread)
	: item_component(thread){}

winp::menu::separator::separator(ui::tree &parent)
	: item_component(parent){}

winp::menu::separator::~separator() = default;

bool winp::menu::separator::validate_child_insert_(const ui::object &child, std::size_t index) const{
	return false;
}

bool winp::menu::separator::validate_child_remove_(const ui::object &child) const{
	return false;
}

UINT winp::menu::separator::get_persistent_states_() const{
	return MFS_GRAYED;
}

UINT winp::menu::separator::get_types_() const{
	return MFT_SEPARATOR;
}

void winp::menu::separator::generate_id_(std::size_t max_tries){}
