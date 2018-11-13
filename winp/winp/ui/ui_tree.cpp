#include "ui_tree.h"

winp::ui::tree::tree() = default;

winp::ui::tree::tree(thread::object &thread)
	: object(thread){}

winp::ui::tree::~tree() = default;

void winp::ui::tree::add_child(object &child, const std::function<void(object &, bool, std::size_t)> &callback){
	add_child(child, static_cast<std::size_t>(-1), callback);
}

void winp::ui::tree::add_child(object &child, std::size_t index, const std::function<void(object &, bool, std::size_t)> &callback){
	auto pchild = &child;
	thread_.queue.post([this, index, pchild, callback]{
		auto pindex = add_child_(*pchild, index);
		if (callback != nullptr)
			callback(*this, (pindex != static_cast<std::size_t>(-1)), pindex);
	}, thread::queue::send_priority, id_);
}

void winp::ui::tree::remove_child(object &child, const std::function<void(object &, bool)> &callback){
	auto pchild = &child;
	thread_.queue.post([this, pchild, callback]{
		auto result = erase_child_(*pchild);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::tree::remove_child_at(std::size_t index, const std::function<void(object &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = erase_child_at_(index);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

std::size_t winp::ui::tree::find_child(const object &child, const std::function<void(std::size_t)> &callback) const{
	auto pchild = &child;
	if (callback != nullptr){
		thread_.queue.post([this, pchild, callback]{ callback(find_child_(*pchild)); }, thread::queue::send_priority, id_);
		return static_cast<std::size_t>(-1);
	}

	return thread_.queue.add([this, pchild]{ return find_child_(*pchild); }, thread::queue::send_priority, id_).get();
}

winp::ui::object *winp::ui::tree::get_child_at(std::size_t index, const std::function<void(object *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_child_at_(index)); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([=]{ return get_child_at_(index); }, thread::queue::send_priority, id_).get();
}

void winp::ui::tree::traverse_children(const std::function<void(object *)> &callback, bool post) const{
	if (post){
		thread_.queue.post([=]{
			for (auto child : children_)
				callback(child);
		}, thread::queue::send_priority, id_);
	}
	else{//Wait for traversal
		thread_.queue.add([=]{
			for (auto child : children_)
				callback(child);
		}, thread::queue::send_priority, id_).get();
	}
}

bool winp::ui::tree::validate_child_insert_(const object &child, std::size_t index) const{
	return (child.thread_.get_local_id() == thread_.get_local_id());
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

void winp::ui::tree::child_inserted_(object &child, std::size_t index){}

bool winp::ui::tree::validate_child_remove_(const object &child) const{
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
	auto child = *it;

	children_.erase(it);
	child_removed_(*child, index);

	return true;
}

void winp::ui::tree::child_removed_(object &child, std::size_t index){}

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
