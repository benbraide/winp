#include "../app/app_object.h"
#include "ui_tree.h"

winp::ui::send_message::send_message() = default;

winp::ui::send_message::send_message(const send_message &copy)
	: target_(copy.target_){
	if (target_ != nullptr)
		init_();
}

winp::ui::send_message &winp::ui::send_message::operator=(const send_message &copy){
	if ((target_ = copy.target_) != nullptr && target.getter_ == nullptr)
		init_();
	return *this;
}

void winp::ui::send_message::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &result){
			if (!app::object::is_native_handle(target_)){
				auto object_target = reinterpret_cast<object *>(target_);
				if (!app::object::is_native_handle(object_target->handle)){
					*static_cast<LRESULT *>(buf) = object_target->owner_->queue->add([this, object_target]() -> LRESULT{
						message::basic::info_type info;
						{//Populate info
							info.code = code.m_value_;
							info.wparam = wparam.m_value_;
							info.lparam = lparam.m_value_;
						}

						message::basic msg(object_target, info);
						object_target->handle_message_(msg);

						return msg.result;
					}, thread::queue::send_priority).get();
				}
				else
					*static_cast<LRESULT *>(buf) = SendMessageW(object_target->handle, code.m_value_, wparam.m_value_, lparam.m_value_);
			}
			else//Target is a native handle
				*static_cast<LRESULT *>(buf) = SendMessageW(target_, code.m_value_, wparam.m_value_, lparam.m_value_);
		}
		else if (&prop == &target)
			*static_cast<HWND *>(buf) = target_;
	};

	target.init_(nullptr, nullptr, getter);
	result.init_(nullptr, nullptr, getter);
}

winp::ui::post_message::post_message() = default;

winp::ui::post_message::post_message(const post_message &copy)
	: target_(copy.target_){
	if (target_ != nullptr)
		init_();
}

winp::ui::post_message &winp::ui::post_message::operator=(const post_message &copy){
	if ((target_ = copy.target_) != nullptr && target.getter_ == nullptr)
		init_();
	return *this;
}

void winp::ui::post_message::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &result){
			if (!app::object::is_native_handle(target_)){
				auto object_target = reinterpret_cast<object *>(target_);
				if (!app::object::is_native_handle(object_target->handle)){
					object_target->owner_->queue->post([this, object_target]{
						message::basic::info_type info;
						{//Populate info
							info.code = code.m_value_;
							info.wparam = wparam.m_value_;
							info.lparam = lparam.m_value_;
						}

						message::basic msg(object_target, info);
						object_target->handle_message_(msg);
					});
					*static_cast<bool *>(buf) = true;
				}
				else
					*static_cast<bool *>(buf) = (PostMessageW(object_target->handle, code.m_value_, wparam.m_value_, lparam.m_value_) != FALSE);
			}
			else//Target is a native handle
				*static_cast<bool *>(buf) = (PostMessageW(target_, code.m_value_, wparam.m_value_, lparam.m_value_) != FALSE);
		}
		else if (&prop == &target)
			*static_cast<HWND *>(buf) = target_;
	};

	target.init_(nullptr, nullptr, getter);
	result.init_(nullptr, nullptr, getter);
}

winp::ui::object::object(thread::object &thread)
	: item(thread), parent_(nullptr), index_(static_cast<std::size_t>(-1)){
	init_();
}

winp::ui::object::object(tree &parent)
	: item(*parent.owner_), parent_(&parent), index_(static_cast<std::size_t>(-1)){
	init_();
}

winp::ui::object::~object() = default;

void winp::ui::object::init_(){
	ancestors.m_value_.init_([this]() -> tree *{//begin
		return parent_;
	}, [this](tree *current) -> tree *{//next
		return current->parent;
	});

	siblings.m_value_.init_([this]() -> object *{//begin
		return owner_->queue->add([this]{ return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(0)); }, thread::queue::send_priority).get();
	}, [this](object *current){//next
		return owner_->queue->add([&]() -> object *{
			if (parent_ == nullptr || current->parent != parent_)
				return nullptr;

			std::size_t index = current->get_index_();
			auto value = parent_->get_child_at_(index + 1u);

			return ((value == this) ? parent_->get_child_at_(index + 2u) : value);
		}, thread::queue::send_priority).get();
	});

	auto setter = [this](const prop::base &prop, const void *value, std::size_t index){
		if (&prop == &parent){
			auto tval = *static_cast<tree **>(const_cast<void *>(value));
			owner_->queue->post([=]{
				change_parent_(tval, get_index_());
			}, thread::queue::send_priority);
		}
		else if (&prop == &this->index){
			auto tval = *static_cast<const std::size_t *>(value);
			owner_->queue->post([=]{
				change_index_(tval);
			}, thread::queue::send_priority);
		}
		else if (&prop == &previous_sibling){
			auto tval = *static_cast<object **>(const_cast<void *>(value));
			owner_->queue->post([=]{
				set_previous_sibling_(tval);
			}, thread::queue::send_priority);
		}
		else if (&prop == &next_sibling){
			auto tval = *static_cast<object **>(const_cast<void *>(value));
			owner_->queue->post([=]{
				set_next_sibling_(tval);
			}, thread::queue::send_priority);
		}
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t index){
		if (&prop == &parent)
			*static_cast<tree **>(buf) = owner_->queue->add([this]{ return get_parent_(); }, thread::queue::send_priority).get();
		else if (&prop == &this->index)
			*static_cast<std::size_t *>(buf) = owner_->queue->add([this]{ return get_index_(); }, thread::queue::send_priority).get();
		else if (&prop == &previous_sibling)
			*static_cast<object **>(buf) = owner_->queue->add([this]{ return get_previous_sibling_(); }, thread::queue::send_priority).get();
		else if (&prop == &next_sibling)
			*static_cast<object **>(buf) = owner_->queue->add([this]{ return get_next_sibling_(); }, thread::queue::send_priority).get();
	};

	parent.init_(nullptr, setter, getter);
	index.init_(nullptr, setter, getter);

	previous_sibling.init_(nullptr, setter, getter);
	next_sibling.init_(nullptr, setter, getter);

	ancestors.init_(nullptr, nullptr, getter);
	siblings.init_(nullptr, nullptr, getter);
}

void winp::ui::object::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(send_message)){
		static_cast<send_message *>(buf)->target_ = get_handle_();
		static_cast<send_message *>(buf)->init_();
	}
	else if (id == typeid(post_message)){
		static_cast<post_message *>(buf)->target_ = get_handle_();
		static_cast<post_message *>(buf)->init_();
	}
	else if (id == typeid(change_event_type))
		*static_cast<change_event_type *>(buf) = change_event_type(change_event_);
	else if (id == typeid(object *))
		*static_cast<object **>(buf) = this;
	else
		item::do_request_(buf, id);
}

void winp::ui::object::set_parent_(tree *value){
	parent_ = value;
}

winp::ui::tree *winp::ui::object::get_parent_() const{
	return parent_;
}

bool winp::ui::object::validate_parent_change_(tree *value, std::size_t index) const{
	app::object::error = prop::default_error_mapper::value_type::nil;
	if (value != nullptr && value->owner_ != owner_){
		throw_(prop::default_error_mapper::value_type::thread_context_mismatch);
		return false;
	}

	auto current_parent = get_parent_();
	if (!fire_parent_change_event_(true, current_parent, value, index))
		return false;

	if (current_parent != nullptr && !current_parent->validate_child_remove_(*this))
		return false;

	return (value == nullptr || value->validate_child_insert_(*this, index));
}

std::size_t winp::ui::object::change_parent_(tree *value, std::size_t index){
	auto previous_parent = get_parent_();
	if (value == previous_parent)//Same parent
		return change_index_(index);

	if (value == nullptr){
		remove_parent_();
		return static_cast<std::size_t>(-1);
	}

	if (!validate_parent_change_(value, index))
		return static_cast<std::size_t>(-1);

	auto previous_index = get_index_();
	if (previous_parent != nullptr && !previous_parent->remove_child_(*this))
		return static_cast<std::size_t>(-1);//Failed to remove from parent

	try{//Revert parent on exception
		set_parent_(value);
		value = get_parent_();

		if (value != nullptr && (index = value->insert_child_(*this, index)) == static_cast<std::size_t>(-1))
			set_parent_(nullptr);//Failed to insert into parent
	}
	catch (...){//Failed to insert into parent
		set_parent_(nullptr);
		throw;//Forward exception
	}

	if (value != previous_parent){
		parent_changed_(previous_parent, previous_index);
		index_changed_(static_cast<std::size_t>(-1));
	}

	return index;
}

bool winp::ui::object::remove_parent_(){
	auto previous_parent = get_parent_();
	if (previous_parent == nullptr)
		return true;

	if (!validate_parent_change_(nullptr, static_cast<std::size_t>(-1)))
		return false;

	auto previous_index = get_index_();
	if (previous_parent != nullptr && !previous_parent->remove_child_(*this))
		return false;

	set_parent_(nullptr);
	parent_changed_(previous_parent, previous_index);
	index_changed_(static_cast<std::size_t>(-1));

	return true;
}

void winp::ui::object::parent_changed_(tree *previous_parent, std::size_t previous_index){
	auto parent = get_parent_();
	fire_parent_change_event_(false, previous_parent, parent, (previous_index = get_index_()));
}

bool winp::ui::object::validate_index_change_(std::size_t value) const{
	if (!fire_index_change_event_(true, get_index_(), value))
		return false;

	auto parent = get_parent_();
	return (parent == nullptr || parent->validate_child_index_change_(*this, value));
}

std::size_t winp::ui::object::change_index_(std::size_t value){
	auto previous_index = get_index_();
	if (value == previous_index)
		return value;

	if (!validate_index_change_(value))
		return static_cast<std::size_t>(-1);

	if (parent_ != nullptr){
		value = parent_->change_child_index_(*this, value);
		if (value != static_cast<std::size_t>(-1) && value != previous_index)
			index_changed_(previous_index);
	}
	else
		index_ = value;

	return value;
}

void winp::ui::object::index_changed_(std::size_t previous){
	auto index = get_index_();
	fire_index_change_event_(false, previous, index);
}

std::size_t winp::ui::object::get_index_() const{
	return ((parent_ == nullptr) ? index_ : parent_->find_child_(*this));
}

void winp::ui::object::set_previous_sibling_(object *target){
	if (target != nullptr && target->parent_ != nullptr)
		change_parent_(target->parent_, (target->get_index_() + 1));
}

winp::ui::object *winp::ui::object::get_previous_sibling_() const{
	if (parent_ == nullptr)
		return nullptr;

	auto index = get_index_();
	return ((index == 0u) ? nullptr : parent_->get_child_at_(index - 1u));
}

void winp::ui::object::set_next_sibling_(object *target){
	if (target != nullptr && target->parent_ != nullptr)
		change_parent_(target->parent_, target->get_index_());
}

winp::ui::object *winp::ui::object::get_next_sibling_() const{
	return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(get_index_() + 1u));
}

bool winp::ui::object::handle_message_(message::basic &info){
	return false;
}

void winp::ui::object::fire_ancestor_change_event_(tree *value, std::size_t index) const{
	ancestor_change_info info{
		value,
		index
	};

	event::change<void, unsigned __int64> e(ancestor_change_id, info, const_cast<object *>(this));
	change_event_.fire_(e);
}

bool winp::ui::object::fire_parent_change_event_(bool is_changing, tree *current_value, tree *&value, std::size_t &index) const{
	parent_change_info info{
		is_changing,
		current_value,
		value,
		index
	};

	event::change<void, unsigned __int64> e(parent_change_id, info, const_cast<object *>(this));
	change_event_.fire_(e);
	
	if (is_changing){//Update values
		if (info.new_parent == nullptr || info.new_parent->owner_ == owner_)
			value = info.new_parent;
		index = info.index;
	}
	else//Notify ancestor change
		fire_ancestor_change_event_(value, 0u);

	return (is_changing && !e.prevent_default);
}

bool winp::ui::object::fire_index_change_event_(bool is_changing, std::size_t previous_value, std::size_t &value) const{
	index_change_info info{
		is_changing,
		previous_value,
		value
	};

	event::change<void, unsigned __int64> e(index_change_id, info, const_cast<object *>(this));
	change_event_.fire_(e);

	if (is_changing)//Update value
		value = info.current_index;

	return (is_changing && !e.prevent_default);
}

void winp::ui::object::fire_sibling_change_event_(object &sibling, std::size_t previous_index, std::size_t current_index) const{
	sibling_change_info info{
		&sibling,
		previous_index,
		current_index
	};

	event::change<void, unsigned __int64> e(sibling_change_id, info, const_cast<object *>(this));
	change_event_.fire_(e);
}
