#include "../app/app_object.h"

winp::menu::group::group() = default;

winp::menu::group::group(thread::object &thread)
	: surface(thread){}

winp::menu::group::group(menu::object &parent)
	: surface(parent.get_thread()){
	change_parent_(&parent);
}

winp::menu::group::~group(){
	destruct();
}

std::size_t winp::menu::group::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_absolute_index_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_index_()); }, thread::queue::send_priority, id_);
		return static_cast<std::size_t>(-1);
	}

	return thread_.queue.execute([this]{ return get_absolute_index_(); }, thread::queue::send_priority, id_);
}

winp::menu::item_component *winp::menu::group::find_component(UINT id, const std::function<void(menu::item_component *)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = find_component_(id, nullptr);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(find_component_(id, nullptr)); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([=]{ return find_component_(id, nullptr); }, thread::queue::send_priority, id_);
}

winp::menu::item_component *winp::menu::group::get_component_at_absolute_index(std::size_t index, const std::function<void(menu::item_component *)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_component_at_absolute_index_(index);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_component_at_absolute_index_(index)); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([=]{ return get_component_at_absolute_index_(index); }, thread::queue::send_priority, id_);
}

void winp::menu::group::destruct_(){
	destroy_();
	surface::destruct_();
}

bool winp::menu::group::create_(){
	if (!children_.empty()){
		for (auto child : children_)
			child->create_();
	}
	return true;
}

bool winp::menu::group::destroy_(){
	if (!children_.empty()){
		for (auto child : children_)
			child->destroy_();
	}
	return true;
}

HANDLE winp::menu::group::get_handle_() const{
	auto parent = get_parent_();
	return ((parent == nullptr) ? nullptr : parent->get_handle_());
}

UINT winp::menu::group::get_types_(std::size_t index) const{
	auto parent = dynamic_cast<menu::tree *>(get_parent_());
	return ((parent == nullptr) ? 0u : parent->get_types_(index));
}

UINT winp::menu::group::get_states_(std::size_t index) const{
	auto parent = dynamic_cast<menu::tree *>(get_parent_());
	return ((parent == nullptr) ? 0u : parent->get_states_(index));
}

std::size_t winp::menu::group::get_absolute_index_of_(const menu::component &child, bool skip_this) const{
	menu::tree *tree_child;
	std::size_t index = (skip_this ? 0u : get_absolute_index_()), inner_index;
	if (index == static_cast<std::size_t>(-1))
		index = 0u;

	for (auto pchild : children_){
		if (dynamic_cast<menu::component *>(pchild) == &child)
			return index;

		if ((tree_child = dynamic_cast<menu::tree *>(pchild)) != nullptr){
			if ((inner_index = tree_child->get_absolute_index_of_(child, true)) == static_cast<std::size_t>(-1))
				index += tree_child->get_count_();
			else
				return (index + inner_index);
		}
		else
			++index;
	}

	return static_cast<std::size_t>(-1);
}

winp::menu::item_component *winp::menu::group::find_component_(UINT id, item_component *exclude) const{
	menu::item_component *item;
	menu::tree *tree_child;

	for (auto child : children_){
		if ((tree_child = dynamic_cast<menu::tree *>(child)) != nullptr){
			if ((item = tree_child->find_component_(id, exclude)) != nullptr)
				return item;
		}
		else if ((item = dynamic_cast<menu::item *>(child)) != nullptr && item != exclude && item->local_id_ == id)
			return item;
	}

	return nullptr;
}

winp::menu::item_component *winp::menu::group::get_component_at_absolute_index_(std::size_t index) const{
	menu::item_component *item;
	menu::tree *tree_child;

	for (auto child : children_){
		if ((tree_child = dynamic_cast<menu::tree *>(child)) != nullptr){
			if ((item = tree_child->get_component_at_absolute_index_(index)) == nullptr)
				index -= tree_child->get_count_();
			else
				return item;
		}
		else if (index > 0u)
			--index;
		else
			return dynamic_cast<menu::item_component *>(child);
	}

	return nullptr;
}

std::size_t winp::menu::group::get_count_() const{
	std::size_t count = 0u;
	menu::tree *tree_child;

	for (auto child : children_){
		if ((tree_child = dynamic_cast<menu::tree *>(child)) != nullptr)
			count += tree_child->get_count_();
		else
			++count;
	}

	return count;
}

bool winp::menu::group::handle_parent_change_event_(event::tree &e){
	if (e.get_attached_parent() != nullptr && dynamic_cast<menu::tree *>(e.get_attached_parent()) == nullptr)
		return false;

	if (!children_.empty()){
		for (auto child : children_){
			if (auto handler = dynamic_cast<event::tree_handler *>(child); handler != nullptr)
				handler->handle_parent_change_event_(e);
		}
	}

	return true;
}

bool winp::menu::group::handle_child_insert_event_(event::tree &e){
	return (dynamic_cast<menu::component *>(e.get_target()) != nullptr);
}

void winp::menu::group::handle_parent_changed_event_(event::tree &e){
	if (!children_.empty()){
		for (auto child : children_){
			if (auto handler = dynamic_cast<event::tree_handler *>(child); handler != nullptr)
				handler->handle_parent_changed_event_(e);
		}
	}
}

void winp::menu::group::handle_index_changed_event_(event::tree &e){
	if (!children_.empty()){
		for (auto child : children_){
			if (auto handler = dynamic_cast<event::tree_handler *>(child); handler != nullptr)
				handler->handle_index_changed_event_(e);
		}
	}
}

std::size_t winp::menu::group::get_absolute_index_() const{
	auto parent = dynamic_cast<menu::tree *>(get_parent_());
	return ((parent == nullptr) ? get_index_() : parent->get_absolute_index_of_(*this, false));
}

void winp::menu::group::update_children_types_(){
	menu::group *group_child;
	menu::item_component *item_child;

	for (auto child : children_){
		if ((item_child = dynamic_cast<menu::item_component *>(child)) != nullptr)
			item_child->update_types_();
		else if ((group_child = dynamic_cast<menu::group *>(child)) != nullptr)
			group_child->update_children_types_();
	}
}
