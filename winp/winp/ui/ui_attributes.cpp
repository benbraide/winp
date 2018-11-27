#include "ui_attributes.h"

winp::ui::parent_change_attribute::~parent_change_attribute() = default;

bool winp::ui::parent_change_attribute::handle_parent_change_validation_(tree *value, std::size_t index) const{
	return true;
}

bool winp::ui::parent_change_attribute::handle_index_change_validation_(std::size_t value) const{
	return true;
}

void winp::ui::parent_change_attribute::handle_parent_changing_(){}

void winp::ui::parent_change_attribute::handle_parent_changed_(tree *previous_parent, std::size_t previous_index){}

void winp::ui::parent_change_attribute::handle_index_changing_(){}

void winp::ui::parent_change_attribute::handle_index_changed_(tree *previous_parent, std::size_t previous_index){}
