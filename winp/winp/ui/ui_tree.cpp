#include "../app/app_object.h"

winp::ui::tree::tree() = default;

winp::ui::tree::tree(thread::object &thread)
	: object(thread){}

winp::ui::tree::~tree() = default;

std::size_t winp::ui::tree::add_child(object &child, const std::function<void(object &, bool, std::size_t)> &callback){
	return add_child(child, static_cast<std::size_t>(-1), callback);
}

std::size_t winp::ui::tree::add_child(object &child, std::size_t index, const std::function<void(object &, bool, std::size_t)> &callback){
	if (thread_.is_thread_context()){
		auto pindex = add_child_(child, index);
		if (callback != nullptr)
			callback(*this, (pindex != static_cast<std::size_t>(-1)), pindex);
		return pindex;
	}

	auto pchild = &child;
	thread_.queue.post([this, index, pchild, callback]{
		auto pindex = add_child_(*pchild, index);
		if (callback != nullptr)
			callback(*this, (pindex != static_cast<std::size_t>(-1)), pindex);
	}, thread::queue::send_priority, id_);

	return static_cast<std::size_t>(-1);
}

bool winp::ui::tree::remove_child(object &child, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = erase_child_(child);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	auto pchild = &child;
	thread_.queue.post([this, pchild, callback]{
		auto result = erase_child_(*pchild);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::tree::remove_child_at(std::size_t index, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = erase_child_at_(index);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = erase_child_at_(index);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

std::size_t winp::ui::tree::find_child(const object &child, const std::function<void(std::size_t)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = find_child_(child);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	auto pchild = &child;
	if (callback != nullptr){
		thread_.queue.post([this, pchild, callback]{ callback(find_child_(*pchild)); }, thread::queue::send_priority, id_);
		return static_cast<std::size_t>(-1);
	}

	return thread_.queue.execute([this, pchild]{ return find_child_(*pchild); }, thread::queue::send_priority, id_);
}

winp::ui::object *winp::ui::tree::get_child_at(std::size_t index, const std::function<void(object *)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_child_at_(index);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_child_at_(index)); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([=]{ return get_child_at_(index); }, thread::queue::send_priority, id_);
}

void winp::ui::tree::traverse_children(const std::function<void(object *)> &callback, bool post) const{
	if (thread_.is_thread_context()){
		for (auto child : children_)
			callback(child);
	}
	else if (post){
		thread_.queue.post([=]{
			for (auto child : children_)
				callback(child);
		}, thread::queue::send_priority, id_);
	}
	else{//Wait for traversal
		thread_.queue.execute([=]{
			for (auto child : children_)
				callback(child);
		}, thread::queue::send_priority, id_);
	}
}

bool winp::ui::tree::validate_child_insert_(const object &child, std::size_t index) const{
	return (child.thread_.get_local_id() == thread_.get_local_id());
}

std::size_t winp::ui::tree::add_child_(object &child, std::size_t index){
	return child.change_parent_(this, index);
}

std::size_t winp::ui::tree::insert_child_(object &child, std::size_t index){
	auto previous_parent = child.get_parent_();
	if (previous_parent != nullptr)
		return static_cast<std::size_t>(-1);

	auto previous_index = child.get_index_();
	if (index >= children_.size()){
		children_.push_back(&child);
		index = (children_.size() - 1u);
	}
	else
		children_.insert(std::next(children_.begin(), index), &child);

	child.set_parent_(this);
	child_inserted_(child, previous_parent, previous_index);
	child_index_changed_(child, previous_parent, previous_index);

	child_change_info info{
		&child,
		previous_parent,
		previous_index
	};

	dispatch_message_(WINP_WM_CHILD_INSERTED, reinterpret_cast<WPARAM>(&info), 0, false);
	dispatch_message_(WINP_WM_CHILD_INDEX_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);

	return index;
}

void winp::ui::tree::child_inserted_(object &child, tree *previous_parent, std::size_t previous_index){
	child_change_info info{
		&child,
		previous_parent,
		previous_index
	};
	dispatch_message_(WINP_WM_CHILD_INSERTED, reinterpret_cast<WPARAM>(&info), 0);
}

bool winp::ui::tree::validate_child_remove_(const object &child) const{
	return true;
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

	child->parent_changing_();
	children_.erase(it);
	child->set_parent_(nullptr);

	return true;
}

void winp::ui::tree::child_removed_(object &child, std::size_t previous_index){
	child_change_info info{
		&child,
		this,
		previous_index
	};
	dispatch_message_(WINP_WM_CHILD_REMOVED, reinterpret_cast<WPARAM>(&info), 0);
}

bool winp::ui::tree::validate_child_index_change_(const object &child, std::size_t index) const{
	return true;
}

std::size_t winp::ui::tree::change_child_index_(object &child, std::size_t index){
	if (child.get_parent_() != this || children_.empty())
		return static_cast<std::size_t>(-1);

	auto it = std::find(children_.begin(), children_.end(), &child);
	if (it != children_.end())
		return static_cast<std::size_t>(-1);

	auto adjusted_index = ((index >= children_.size()) ? (children_.size() - 1u) : index);
	std::size_t previous_index = std::distance(children_.begin(), it);

	if (previous_index == adjusted_index)
		return adjusted_index;//No changes

	(*it)->index_changing_();
	children_.erase(it);

	if (index < children_.size())
		children_.insert(std::next(children_.begin(), index), &child);
	else
		children_.push_back(&child);

	child_index_changed_(child, this, previous_index);
	return adjusted_index;
}

void winp::ui::tree::child_index_changed_(object &child, tree *previous_parent, std::size_t previous_index){
	child_change_info info{
		&child,
		previous_parent,
		previous_index
	};
	dispatch_message_(WINP_WM_CHILD_INDEX_CHANGED, reinterpret_cast<WPARAM>(&info), 0);
}

std::size_t winp::ui::tree::find_child_(const object &child) const{
	return ((child.get_parent_() == this) ? std::distance(children_.begin(), std::find(children_.begin(), children_.end(), &child)) : static_cast<std::size_t>(-1));
}

winp::ui::object *winp::ui::tree::get_child_at_(std::size_t index) const{
	return ((index < children_.size()) ? *std::next(children_.begin(), index) : nullptr);
}
