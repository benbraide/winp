#include "ui_tree.h"

winp::ui::object::object()
	: parent_(nullptr), m_index_(static_cast<std::size_t>(-1)){

	ancestors.m_value_.init_([this]() -> tree *{//begin
		return parent_;
	}, [this](tree *current) -> tree *{//next
		return current->parent;
	});

	siblings.m_value_.init_([this]() -> object *{//begin
		return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(0));
	}, [this](object *current) -> object *{//next
		if (parent_ == nullptr || current->parent != parent_)
			return nullptr;

		std::size_t index = current->index_();
		auto value = parent_->get_child_at_(index + 1u);

		return ((value == this) ? parent_->get_child_at_(index + 2u) : value);
	});

	auto setter = [this](const prop::base<object> &prop, const void *value, std::size_t index){
		if (&prop == &parent)
			change_parent_(*static_cast<tree **>(const_cast<void *>(value)), index_());
		else if (&prop == &this->index)
			change_index_(*static_cast<const std::size_t *>(value));
		else if (&prop == &previous_sibling)
			set_previous_sibling_(*static_cast<object **>(const_cast<void *>(value)));
		else if (&prop == &next_sibling)
			set_next_sibling_(*static_cast<object **>(const_cast<void *>(value)));
	};

	auto getter = [this](const prop::base<object> &prop, void *buf, std::size_t index){
		if (&prop == &parent)
			*static_cast<tree **>(buf) = parent_;
		else if (&prop == &this->index)
			*static_cast<std::size_t *>(buf) = index_();
		else if (&prop == &previous_sibling)
			*static_cast<object **>(buf) = get_previous_sibling_();
		else if (&prop == &next_sibling)
			*static_cast<object **>(buf) = get_next_sibling_();
	};

	parent.init_(*this, nullptr, setter, getter, &error);
	index.init_(*this, nullptr, setter, getter, &error);

	previous_sibling.init_(*this, nullptr, setter, getter, &error);
	next_sibling.init_(*this, nullptr, setter, getter, &error);

	ancestors.init_(*this, nullptr, nullptr, getter, &error);
	siblings.init_(*this, nullptr, nullptr, getter, &error);
}

winp::ui::object::~object() = default;

bool winp::ui::object::validate_parent_change_(tree *value, std::size_t index) const{
	return true;
}

std::size_t winp::ui::object::change_parent_(tree *value, std::size_t index){
	if (value == parent_)//Same parent
		return change_index_(index);

	if (value == nullptr){
		remove_parent_();
		return static_cast<std::size_t>(-1);
	}

	if (!validate_parent_change_(value, index))
		return static_cast<std::size_t>(-1);

	auto previous_parent = parent_;
	auto previous_index = index_();

	if (parent_ != nullptr && !parent_->remove_child_(*this))
		return static_cast<std::size_t>(-1);//Failed to remove from parent

	try{//Revert parent on exception
		if ((parent_ = value) != nullptr && (index = parent_->insert_child_(*this, index)) == static_cast<std::size_t>(-1))
			parent_ = nullptr;//Failed to insert into parent
	}
	catch (...){//Failed to insert into parent
		parent_ = nullptr;
		throw;//Forward exception
	}

	if (parent_ != previous_parent)
		parent_changed_(previous_parent, previous_index);

	return index;
}

bool winp::ui::object::remove_parent_(){
	if (!validate_parent_change_(nullptr, static_cast<std::size_t>(-1)))
		return false;

	auto previous_parent = parent_;
	auto previous_index = index_();

	if (parent_ != nullptr && !parent_->remove_child_(*this))
		return false;

	parent_ = nullptr;
	parent_changed_(previous_parent, previous_index);

	return true;
}

void winp::ui::object::parent_changed_(tree *previous_parent, std::size_t previous_index){}

bool winp::ui::object::validate_index_change_(std::size_t value) const{
	return true;
}

std::size_t winp::ui::object::change_index_(std::size_t value){
	auto previous_index = index_();
	if (value == previous_index || !validate_index_change_(value))
		return static_cast<std::size_t>(-1);

	if (parent_ != nullptr){
		value = parent_->change_child_index_(*this, value);
		if (value != static_cast<std::size_t>(-1) && value != previous_index)
			index_changed_(previous_index);
	}
	else
		m_index_ = value;

	return value;
}

void winp::ui::object::index_changed_(std::size_t previous){}

std::size_t winp::ui::object::index_() const{
	return ((parent_ == nullptr) ? m_index_ : parent_->find_child_(*this));
}

void winp::ui::object::set_previous_sibling_(object *target){
	if (target != nullptr && target->parent_ != nullptr)
		change_parent_(target->parent_, (target->index_() + 1));
}

winp::ui::object *winp::ui::object::get_previous_sibling_() const{
	if (parent_ == nullptr)
		return nullptr;

	auto index = index_();
	return ((index == 0u) ? nullptr : parent_->get_child_at_(index - 1u));
}

void winp::ui::object::set_next_sibling_(object *target){
	if (target != nullptr && target->parent_ != nullptr)
		change_parent_(target->parent_, target->index_());
}

winp::ui::object *winp::ui::object::get_next_sibling_() const{
	return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(index_() + 1u));
}
