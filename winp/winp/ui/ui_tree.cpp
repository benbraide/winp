#include "../app/app_object.h"

winp::ui::tree::tree() = default;

winp::ui::tree::tree(thread::object &thread)
	: object(thread){}

winp::ui::tree::~tree() = default;

std::size_t winp::ui::tree::add_child(object &child, const std::function<void(thread::item &, std::size_t)> &callback){
	return add_child(child, static_cast<std::size_t>(-1), callback);
}

std::size_t winp::ui::tree::add_child(object &child, std::size_t index, const std::function<void(thread::item &, std::size_t)> &callback){
	return execute_or_post_task([pchild = &child, index, callback, this]{
		return pass_value_to_callback_(callback, add_child_(*pchild, index));
	});
}

bool winp::ui::tree::remove_child(object &child, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([pchild = &child, callback, this]{
		return pass_value_to_callback_(callback, erase_child_(*pchild));
	});
}

bool winp::ui::tree::remove_child_at(std::size_t index, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, erase_child_at_(index));
	});
}

std::size_t winp::ui::tree::find_child(const object &child, const std::function<void(std::size_t)> &callback) const{
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, find_child_(child));
	});
}

winp::ui::object *winp::ui::tree::get_child_at(std::size_t index, const std::function<void(object *)> &callback) const{
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, get_child_at_(index));
	});
}

void winp::ui::tree::traverse_children(const std::function<void(object *)> &callback, bool post) const{
	execute_or_post_([=]{
		for (auto child : children_)
			callback(child);
	}, post);
}

std::size_t winp::ui::tree::add_child_(object &child, std::size_t index){
	return child.change_parent_(this, index);
}

std::size_t winp::ui::tree::insert_child_(object &child, std::size_t index){
	auto previous_parent = child.get_parent_();
	if (previous_parent != nullptr)
		return static_cast<std::size_t>(-1);

	tree_change_info info{
		&child,
		nullptr,
		index
	};

	if (child.thread_.get_local_id() != thread_.get_local_id() || dispatch_message_(WINP_WM_CHILD_INSERTING, reinterpret_cast<WPARAM>(&info), 0) != 0)
		return static_cast<std::size_t>(-1);

	auto previous_index = child.get_index_();
	if (index >= children_.size()){
		children_.push_back(&child);
		index = (children_.size() - 1u);
	}
	else
		children_.insert(std::next(children_.begin(), index), &child);

	child.set_parent_(this);
	info.index = static_cast<std::size_t>(-1);

	dispatch_message_(WINP_WM_CHILD_INSERTED, reinterpret_cast<WPARAM>(&info), 0, false);
	dispatch_message_(WINP_WM_CHILD_INDEX_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);

	return index;
}

bool winp::ui::tree::erase_child_(object &child){
	return ((child.get_parent_() == this) ? child.remove_parent_() : false);
}

bool winp::ui::tree::remove_child_(object &child){
	return ((child.get_parent_() == this) ? remove_child_at_(std::distance(children_.begin(), std::find(children_.begin(), children_.end(), &child))) : false);
}

bool winp::ui::tree::erase_child_at_(std::size_t index){
	return ((index < children_.size()) ? (*std::next(children_.begin(), index))->remove_parent_() : false);
}

bool winp::ui::tree::remove_child_at_(std::size_t index){
	if (index >= children_.size())
		return false;

	auto it = std::next(children_.begin(), index);
	auto child = *it;

	tree_change_info info{
		child,
		nullptr,
		static_cast<std::size_t>(-1)
	};

	if (dispatch_message_(WINP_WM_CHILD_REMOVING, reinterpret_cast<WPARAM>(&info), 0, false) != 0)
		return false;

	children_.erase(it);
	child->set_parent_(nullptr);
	dispatch_message_(WINP_WM_CHILD_REMOVED, reinterpret_cast<WPARAM>(&info), 0, false);

	return true;
}

std::size_t winp::ui::tree::change_child_index_(object &child, std::size_t index){
	if (child.get_parent_() != this || children_.empty())
		return static_cast<std::size_t>(-1);

	auto it = std::find(children_.begin(), children_.end(), &child);
	if (it != children_.end())
		return static_cast<std::size_t>(-1);

	tree_change_info info{
		&child,
		nullptr,
		index
	};

	if (dispatch_message_(WINP_WM_CHILD_INDEX_CHANGING, reinterpret_cast<WPARAM>(&info), 0, false) != 0)
		return static_cast<std::size_t>(-1);

	auto adjusted_index = ((index >= children_.size()) ? (children_.size() - 1u) : index);
	std::size_t previous_index = std::distance(children_.begin(), it);

	if (previous_index == adjusted_index)
		return adjusted_index;//No changes

	children_.erase(it);
	if (index < children_.size())
		children_.insert(std::next(children_.begin(), index), &child);
	else
		children_.push_back(&child);

	info.index = adjusted_index;
	dispatch_message_(WINP_WM_CHILD_INDEX_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);

	return adjusted_index;
}

std::size_t winp::ui::tree::find_child_(const object &child) const{
	return ((child.get_parent_() == this) ? std::distance(children_.begin(), std::find(children_.begin(), children_.end(), &child)) : static_cast<std::size_t>(-1));
}

winp::ui::object *winp::ui::tree::get_child_at_(std::size_t index) const{
	return ((index < children_.size()) ? *std::next(children_.begin(), index) : nullptr);
}
