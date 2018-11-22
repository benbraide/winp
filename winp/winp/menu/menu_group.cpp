#include "../app/app_object.h"

winp::menu::group::group() = default;

winp::menu::group::group(thread::object &thread)
	: surface(thread){}

winp::menu::group::group(menu::object &parent)
	: surface(parent.get_thread()){
	change_parent_(&parent);
}

winp::menu::group::~group(){
	destruct_();
}

std::size_t winp::menu::group::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_index_()); }, thread::queue::send_priority, id_);
		return static_cast<std::size_t>(-1);
	}

	return thread_.queue.add([this]{ return get_absolute_index_(); }, thread::queue::send_priority, id_).get();
}

winp::menu::item *winp::menu::group::find_component(WORD id, const std::function<void(menu::item *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(find_component_(id)); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([=]{ return find_component_(id); }, thread::queue::send_priority, id_).get();
}

winp::menu::component *winp::menu::group::get_component_at_absolute_index(std::size_t index, const std::function<void(menu::component *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_component_at_absolute_index_(index)); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([=]{ return get_component_at_absolute_index_(index); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::group::create_(){
	return true;
}

bool winp::menu::group::destroy_(){
	return true;
}

bool winp::menu::group::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return (surface::validate_parent_change_(value, index) && dynamic_cast<menu::item *>(value) != nullptr);
}

void winp::menu::group::parent_changed_(ui::tree *previous_parent, std::size_t previous_index){

}

UINT winp::menu::group::get_types_(std::size_t index) const{
	return 0u;
}

UINT winp::menu::group::get_states_(std::size_t index) const{
	return 0u;
}

std::size_t winp::menu::group::get_absolute_index_of_(const menu::component &child) const{
	menu::tree *tree_child;
	std::size_t index = get_absolute_index_(), inner_index;
	if (index == static_cast<std::size_t>(-1))
		index = 0u;

	for (auto pchild : children_){
		if ((tree_child = dynamic_cast<menu::tree *>(pchild)) != nullptr){
			if ((inner_index = tree_child->get_absolute_index_of_(child)) == static_cast<std::size_t>(-1))
				index += tree_child->get_count_();
			else
				return (index + inner_index);
		}
		else if (dynamic_cast<menu::component *>(pchild) != &child)
			++index;
		else
			return index;
	}

	return static_cast<std::size_t>(-1);
}

winp::menu::item *winp::menu::group::find_component_(WORD id) const{
	menu::item *item;
	menu::tree *tree_child;

	for (auto child : children_){
		if ((tree_child = dynamic_cast<menu::tree *>(child)) != nullptr){
			if ((item = tree_child->find_component_(id)) != nullptr)
				return item;
		}
		else if ((item = dynamic_cast<menu::item *>(child)) != nullptr && item->local_id_ == id)
			return item;
	}

	return nullptr;
}

winp::menu::component *winp::menu::group::get_component_at_absolute_index_(std::size_t index) const{
	menu::component *item;
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
			return dynamic_cast<menu::component *>(child);
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

std::size_t winp::menu::group::get_absolute_index_() const{
	auto parent = dynamic_cast<menu::tree *>(get_parent_());
	return ((parent == nullptr) ? get_index_() : parent->get_absolute_index_of_(*this));
}

void winp::menu::group::destruct_(){
	thread_.queue.add([=]{
		destroy_();
	}, thread::queue::send_priority, id_).get();
}