#include "ui_tree.h"

winp::ui::tree::tree(thread::object &thread)
	: object(thread){
	init_();
}

winp::ui::tree::tree(tree &parent)
	: object(parent){
	init_();
}

winp::ui::tree::~tree() = default;

void winp::ui::tree::init_(){
	auto setter = [this](const prop::base<tree> &prop, const void *value, std::size_t index){
		auto nc_value = const_cast<void *>(value);
		if (&prop == &children){
			switch (index){
			case prop::list_action::action_add:
			{
				auto info = static_cast<std::pair<std::size_t, object *> *>(nc_value);
				if (info->second != nullptr)
					info->first = add_child_(*info->second);
				break;
			}
			case prop::list_action::action_remove:
			{
				auto info = static_cast<std::pair<bool, object *> *>(nc_value);
				if (info->second != nullptr)
					info->first = erase_child_(*info->second);
				break;
			}
			case prop::list_action::action_remove_index:
			{
				auto info = static_cast<std::pair<bool, std::size_t> *>(nc_value);
				info->first = erase_child_at_(info->second);
				break;
			}
			case prop::list_action::action_at:
			{
				auto info = static_cast<std::pair<std::size_t, object *> *>(nc_value);
				info->second = get_child_at_(info->first);
				break;
			}
			case prop::list_action::action_find:
			{
				auto info = static_cast<std::pair<std::size_t, object *> *>(nc_value);
				if (info->second != nullptr)
					info->first = find_child_(*info->second);
				break;
			}
			case prop::list_action::action_first:
				*static_cast<object **>(nc_value) = get_child_at_(0);
				break;
			case prop::list_action::action_last:
				*static_cast<object **>(nc_value) = get_child_at_(children_.size() - 1u);
				break;
			case prop::list_action::action_begin:
				*static_cast<std::list<object *>::iterator *>(nc_value) = children_.begin();
				break;
			case prop::list_action::action_end:
				*static_cast<std::list<object *>::iterator *>(nc_value) = children_.end();
				break;
			case prop::list_action::action_size:
				*static_cast<std::size_t *>(nc_value) = children_.size();
				break;
			default:
				break;
			}
		}
	};

	children.init_(*this, nullptr, setter, nullptr);
}

bool winp::ui::tree::validate_child_insert_(object &child, std::size_t index) const{
	return true;
}

std::size_t winp::ui::tree::add_child_(object &child, std::size_t index){
	return child.change_parent_(this, index);
}

std::size_t winp::ui::tree::insert_child_(object &child, std::size_t index){
	if (!validate_child_insert_(child, index))
		return static_cast<std::size_t>(-1);

	if (index >= children_.size()){
		children_.push_back(&child);
		index = (children_.size() - 1u);
	}
	else
		children_.insert(std::next(children_.begin(), index), &child);

	child_inserted_(child, index);
	return index;
}

void winp::ui::tree::child_inserted_(object &child, std::size_t index){}

bool winp::ui::tree::validate_child_remove_(object &child) const{
	return true;
}

bool winp::ui::tree::erase_child_(object &child){
	return ((child.parent_ == this) ? child.remove_parent_() : false);
}

bool winp::ui::tree::remove_child_(object &child){
	return ((child.parent_ == this) ? remove_child_at_(std::distance(children_.begin(), std::find(children_.begin(), children_.end(), &child))) : false);
}

bool winp::ui::tree::erase_child_at_(std::size_t index){
	return ((index < children_.size()) ? (*std::next(children_.begin(), index))->remove_parent_() : false);
}

bool winp::ui::tree::remove_child_at_(std::size_t index){
	if (index >= children_.size())
		return false;

	auto it = std::next(children_.begin(), index);
	if (!validate_child_remove_(**it))
		return false;

	auto child = *it;
	children_.erase(it);
	child_removed_(*child);

	return true;
}

void winp::ui::tree::child_removed_(object &child){}

bool winp::ui::tree::validate_child_index_change_(object &child, std::size_t index) const{
	return true;
}

std::size_t winp::ui::tree::change_child_index_(object &child, std::size_t index){
	if (child.parent_ != this)
		return static_cast<std::size_t>(-1);

	std::size_t current_index;
	auto it = std::find(children_.begin(), children_.end(), &child);

	if (it != children_.end()){
		current_index = std::distance(children_.begin(), it);
		if (current_index == index || (current_index == (children_.size() - 1u) && index >= children_.size()))
			return index;//No changes
		children_.erase(it);
	}
	else
		current_index = static_cast<std::size_t>(-1);

	if (index >= children_.size()){
		children_.push_back(&child);
		index = (children_.size() - 1u);
	}
	else
		children_.insert(std::next(children_.begin(), index), &child);

	child_index_changed_(child, current_index, index);
	return index;
}

void winp::ui::tree::child_index_changed_(object &child, std::size_t previous_index, std::size_t index){}

std::size_t winp::ui::tree::find_child_(const object &child) const{
	return ((child.parent_ == this) ? std::distance(children_.begin(), std::find(children_.begin(), children_.end(), &child)) : static_cast<std::size_t>(-1));
}

winp::ui::object *winp::ui::tree::get_child_at_(std::size_t index) const{
	return ((index < children_.size()) ? *std::next(children_.begin(), index) : nullptr);
}
