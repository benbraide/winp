#include "../app/app_object.h"

winp::ui::object::object()
	: parent_change_event(*this), handle_(nullptr), parent_(nullptr), index_(static_cast<std::size_t>(-1)){
	init_();
}

winp::ui::object::object(thread::object &thread)
	: item(thread), parent_change_event(*this), handle_(nullptr), parent_(nullptr), index_(static_cast<std::size_t>(-1)){
	init_();
}

winp::ui::object::~object() = default;

void winp::ui::object::create(const std::function<void(object &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = create_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::ui::object::destroy(const std::function<void(object &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = destroy_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

HWND winp::ui::object::get_handle(const std::function<void(HWND)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_handle()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_handle(); }, thread::queue::send_priority, id_).get();
}

void winp::ui::object::set_parent(tree *value, const std::function<void(object &, bool, std::size_t)> &callback){
	thread_.queue.post([=]{
		auto index = change_parent_(value, get_index());
		if (callback != nullptr)
			callback(*this, (index != static_cast<std::size_t>(-1)), index);
	}, thread::queue::send_priority, id_);
}

winp::ui::tree *winp::ui::object::get_parent(const std::function<void(tree *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_parent_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_parent_(); }, thread::queue::send_priority, id_).get();
}

void winp::ui::object::set_index(std::size_t value, const std::function<void(object &, bool, std::size_t)> &callback){
	thread_.queue.post([=]{
		auto index = change_index_(value);
		if (callback != nullptr)
			callback(*this, (index != static_cast<std::size_t>(-1)), index);
	}, thread::queue::send_priority, id_);
}

std::size_t winp::ui::object::get_index(const std::function<void(std::size_t)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_index_()); }, thread::queue::send_priority, id_);
		return static_cast<std::size_t>(-1);
	}

	return thread_.queue.add([this]{ return get_index_(); }, thread::queue::send_priority, id_).get();
}

void winp::ui::object::set_previous_sibling(object *target, const std::function<void(object &, bool)> &callback){
	thread_.queue.post([=]{
		auto value = set_previous_sibling_(target);
		if (callback != nullptr)
			callback(*this, value);
	}, thread::queue::send_priority, id_);
}

winp::ui::object *winp::ui::object::get_previous_sibling(const std::function<void(object *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_previous_sibling_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_previous_sibling_(); }, thread::queue::send_priority, id_).get();
}

void winp::ui::object::set_next_sibling(object *target, const std::function<void(object &, bool)> &callback){
	thread_.queue.post([=]{
		auto value = set_next_sibling_(target);
		if (callback != nullptr)
			callback(*this, value);
	}, thread::queue::send_priority, id_);
}

winp::ui::object *winp::ui::object::get_next_sibling(const std::function<void(object *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_next_sibling_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_next_sibling_(); }, thread::queue::send_priority, id_).get();
}

winp::utility::dynamic_list<winp::ui::tree, winp::ui::object> winp::ui::object::get_ancestors(const std::function<void(utility::dynamic_list<tree, object>)> &callback) const{
	if (callback != nullptr)
		thread_.queue.post([=]{ callback(ancestor_list_); }, thread::queue::send_priority, id_);
	return ancestor_list_;
}

winp::utility::dynamic_list<winp::ui::object, winp::ui::object> winp::ui::object::get_siblings(const std::function<void(utility::dynamic_list<object, object>)> &callback) const{
	if (callback != nullptr)
		thread_.queue.post([=]{ callback(sibling_list_); }, thread::queue::send_priority, id_);
	return sibling_list_;
}

LRESULT winp::ui::object::send_message(UINT msg, const std::function<void(LRESULT)> &callback){
	return do_send_message_(msg, 0, 0, callback);
}

bool winp::ui::object::post_message(UINT msg, const std::function<void(bool)> &callback){
	return do_post_message_(msg, 0, 0, callback);
}

void winp::ui::object::init_(){
	ancestor_list_.init_([this](){//begin
		return thread_.queue.add([this]() -> tree *{ return get_parent_(); }, thread::queue::send_priority, id_).get();
	}, [this](tree *current){//next
		return thread_.queue.add([&]() -> tree *{ return current->get_parent_(); }, thread::queue::send_priority, id_).get();
	});

	sibling_list_.init_([this]() -> object *{//begin
		return thread_.queue.add([this]{ return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(0)); }, thread::queue::send_priority, id_).get();
	}, [this](object *current){//next
		return thread_.queue.add([&]() -> object *{
			if (parent_ == nullptr || current->get_parent_() != parent_)
				return nullptr;

			std::size_t index = current->get_index_();
			auto value = parent_->get_child_at_(index + 1u);

			return ((value == this) ? parent_->get_child_at_(index + 2u) : value);
		}, thread::queue::send_priority, id_).get();
	});
}

bool winp::ui::object::create_(){
	return true;
}

bool winp::ui::object::destroy_(){
	return true;
}

void winp::ui::object::set_handle_(HWND value){
	handle_ = value;
}

HWND winp::ui::object::get_handle_() const{
	return handle_;
}

WNDPROC winp::ui::object::get_default_message_entry_() const{
	auto parent = get_parent_();
	return ((parent == nullptr) ? nullptr : parent->get_default_message_entry_());
}

void winp::ui::object::set_parent_(tree *value){
	parent_ = value;
}

winp::ui::tree *winp::ui::object::get_parent_() const{
	return parent_;
}

bool winp::ui::object::validate_parent_change_(tree *value, std::size_t index) const{
	if (value != nullptr && value->thread_.local_id_ != thread_.local_id_)
		return false;//Threads must match

	auto current_parent = get_parent_();
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

void winp::ui::object::parent_changed_(tree *previous_parent, std::size_t previous_index){}

bool winp::ui::object::validate_index_change_(std::size_t value) const{
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

void winp::ui::object::index_changed_(std::size_t previous){}

std::size_t winp::ui::object::get_index_() const{
	return ((parent_ == nullptr) ? index_ : parent_->find_child_(*this));
}

bool winp::ui::object::set_previous_sibling_(object *target){
	if (target != nullptr && target->parent_ != nullptr)
		return (change_parent_(target->parent_, (target->get_index_() + 1)) != static_cast<std::size_t>(-1));
	return false;
}

winp::ui::object *winp::ui::object::get_previous_sibling_() const{
	if (parent_ == nullptr)
		return nullptr;

	auto index = get_index_();
	return ((index == 0u) ? nullptr : parent_->get_child_at_(index - 1u));
}

bool winp::ui::object::set_next_sibling_(object *target){
	if (target != nullptr && target->parent_ != nullptr)
		return (change_parent_(target->parent_, target->get_index_()) != static_cast<std::size_t>(-1));
	return false;
}

winp::ui::object *winp::ui::object::get_next_sibling_() const{
	return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(get_index_() + 1u));
}

LRESULT winp::ui::object::do_send_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(LRESULT)> &callback){
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(send_message_(msg, wparam, lparam)); }, thread::queue::send_priority, id_);
		return 0;
	}

	return thread_.queue.add([=]{ return send_message_(msg, wparam, lparam); }, thread::queue::send_priority, id_).get();
}

LRESULT winp::ui::object::send_message_(UINT msg, WPARAM wparam, LPARAM lparam){
	auto handle = get_handle_();
	if (handle != nullptr)
		return SendMessageW(handle, msg, wparam, lparam);

	return thread_.surface_manager_.find_dispatcher_(msg)->dispatch_(*this, msg, wparam, lparam, false);
}

bool winp::ui::object::do_post_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(bool)> &callback){
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(post_message_(msg, wparam, lparam)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([=]{ return post_message_(msg, wparam, lparam); }, thread::queue::send_priority, id_).get();
}

bool winp::ui::object::post_message_(UINT msg, WPARAM wparam, LPARAM lparam){
	auto handle = get_handle_();
	if (handle != nullptr)
		return (PostMessageW(handle, msg, wparam, lparam) != FALSE);

	thread_.surface_manager_.find_dispatcher_(msg)->dispatch_(*this, msg, wparam, lparam, false);
	return true;
}

std::size_t winp::ui::object::event_handlers_count_(event::manager_base &ev) const{
	return ev.count_();
}

void winp::ui::object::fire_event_(event::manager_base &ev, event::object &e) const{
	ev.fire_generic_(e);
}

winp::message::dispatcher *winp::ui::object::find_dispatcher_(UINT msg){
	return thread_.surface_manager_.find_dispatcher_(msg);
}

winp::ui::tree *winp::ui::object::get_parent_of_(const object &target){
	return target.get_parent_();
}
