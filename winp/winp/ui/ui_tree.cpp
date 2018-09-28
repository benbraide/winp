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
	auto setter = [this](const prop::base &prop, const void *value, std::size_t index){
		auto nc_value = const_cast<void *>(value);
		if (&prop == &children){
			switch (index){
			case prop::list_action::action_add:
			{
				auto &tval = *static_cast<std::pair<std::size_t, object *> *>(nc_value);
				tval.first = static_cast<std::size_t>(-1);

				owner_->queue->post([=]{
					if (tval.second != nullptr)
						add_child_(*tval.second);
				}, thread::queue::send_priority);

				break;
			}
			case prop::list_action::action_remove:
			{
				auto &tval = *static_cast<std::pair<bool, object *> *>(nc_value);
				tval.first = true;

				owner_->queue->post([=]{
					if (tval.second != nullptr)
						erase_child_(*tval.second);
				}, thread::queue::send_priority);

				break;
			}
			case prop::list_action::action_remove_index:
			{
				auto &tval = *static_cast<std::pair<bool, std::size_t> *>(nc_value);
				tval.first = true;

				owner_->queue->post([=]{
					erase_child_at_(tval.second);
				}, thread::queue::send_priority);

				break;
			}
			case prop::list_action::action_at:
			{
				auto info = static_cast<std::pair<std::size_t, object *> *>(nc_value);
				info->second = owner_->queue->add([&]{ return get_child_at_(info->first); }, thread::queue::send_priority).get();
				break;
			}
			case prop::list_action::action_find:
			{
				auto info = static_cast<std::pair<std::size_t, object *> *>(nc_value);
				if (info->second != nullptr)
					info->first = owner_->queue->add([&]{ return find_child_(*info->second); }, thread::queue::send_priority).get();
				break;
			}
			case prop::list_action::action_first:
				*static_cast<object **>(nc_value) = owner_->queue->add([&]{ return get_child_at_(0); }, thread::queue::send_priority).get();
				break;
			case prop::list_action::action_last:
				*static_cast<object **>(nc_value) = owner_->queue->add([&]{ return get_child_at_(children_.size() - 1u); }, thread::queue::send_priority).get();
				break;
			case prop::list_action::action_begin:
				*static_cast<std::list<object *>::iterator *>(nc_value) = owner_->queue->add([&]{ return children_.begin(); }, thread::queue::send_priority).get();
				break;
			case prop::list_action::action_end:
				*static_cast<std::list<object *>::iterator *>(nc_value) = owner_->queue->add([&]{ return children_.end(); }, thread::queue::send_priority).get();
				break;
			case prop::list_action::action_size:
				*static_cast<std::size_t *>(nc_value) = owner_->queue->add([&]{ return children_.size(); }, thread::queue::send_priority).get();
				break;
			default:
				break;
			}
		}
	};

	children.init_(nullptr, setter, nullptr);
}

void winp::ui::tree::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(tree *))
		*static_cast<tree **>(buf) = this;
	else
		object::do_request_(buf, id);
}

bool winp::ui::tree::validate_child_insert_(const object &child, std::size_t index) const{
	return fire_child_change_event_(true, const_cast<object &>(child), ((index <= children_.size()) ? index : children_.size()));
}

std::size_t winp::ui::tree::add_child_(object &child, std::size_t index){
	return child.change_parent_(this, index);
}

std::size_t winp::ui::tree::insert_child_(object &child, std::size_t index){
	if (index >= children_.size()){
		children_.push_back(&child);
		index = (children_.size() - 1u);
	}
	else
		children_.insert(std::next(children_.begin(), index), &child);

	child_inserted_(child, index);
	return index;
}

void winp::ui::tree::child_inserted_(object &child, std::size_t index){
	fire_child_change_event_(false, child, index);
	fire_child_sibling_change_event_(child, static_cast<std::size_t>(-1), index);
}

bool winp::ui::tree::validate_child_remove_(const object &child) const{
	return fire_child_change_event_(true, const_cast<object &>(child), static_cast<std::size_t>(-1));
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
	auto child = *it;

	children_.erase(it);
	child_removed_(*child, index);

	return true;
}

void winp::ui::tree::child_removed_(object &child, std::size_t index){
	fire_child_change_event_(false, child, static_cast<std::size_t>(-1));
	fire_child_sibling_change_event_(child, index, static_cast<std::size_t>(-1));
}

bool winp::ui::tree::validate_child_index_change_(const object &child, std::size_t index) const{
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

void winp::ui::tree::fire_ancestor_change_event_(tree *value, std::size_t index) const{
	object::fire_ancestor_change_event_(value, index);
	for (auto child : children_)
		child->fire_ancestor_change_event_(value, (index + 1u));
}

bool winp::ui::tree::fire_child_change_event_(bool is_changing, object &child, std::size_t index) const{
	child_change_info info{
		&child,
		index
	};

	return fire_change_event_(child_change_id, info, is_changing);
}

void winp::ui::tree::fire_child_sibling_change_event_(object &child, std::size_t previous_index, std::size_t current_index) const{
	std::size_t target_index = ((previous_index == static_cast<std::size_t>(-1)) ? (current_index + 1u) : previous_index), index = 0u, index_copy;
	for (auto this_child : children_){//Notify siblings
		if (this_child != &child){
			this_child->fire_sibling_change_event_(child, previous_index, current_index);
			if (target_index <= index)
				this_child->fire_index_change_event_(false, (index - 1u), (index_copy = index));
		}

		++index;
	}
}
