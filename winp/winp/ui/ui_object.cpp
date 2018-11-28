#include "../app/app_object.h"

winp::ui::object::object()
	: handle_(nullptr), parent_(nullptr), index_(static_cast<std::size_t>(-1)){
	init_();
}

winp::ui::object::object(thread::object &thread)
	: item(thread), handle_(nullptr), parent_(nullptr), index_(static_cast<std::size_t>(-1)){
	init_();
}

winp::ui::object::~object(){
	destruct();
}

bool winp::ui::object::create(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = create_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = create_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::ui::object::destroy(const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = destroy_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = destroy_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

HANDLE winp::ui::object::get_handle(const std::function<void(HANDLE)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_handle_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_handle_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_handle_(); }, thread::queue::send_priority, id_);
}

std::size_t winp::ui::object::set_parent(tree *value, const std::function<void(object &, bool, std::size_t)> &callback){
	if (thread_.is_thread_context()){
		auto index = change_parent_(value, get_index());
		if (callback != nullptr)
			callback(*this, (index != static_cast<std::size_t>(-1)), index);
		return index;
	}

	thread_.queue.post([=]{
		auto index = change_parent_(value, get_index());
		if (callback != nullptr)
			callback(*this, (index != static_cast<std::size_t>(-1)), index);
	}, thread::queue::send_priority, id_);

	return static_cast<std::size_t>(-1);
}

winp::ui::tree *winp::ui::object::get_parent(const std::function<void(tree *)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_parent_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_parent_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_parent_(); }, thread::queue::send_priority, id_);
}

std::size_t winp::ui::object::set_index(std::size_t value, const std::function<void(object &, bool, std::size_t)> &callback){
	if (thread_.is_thread_context()){
		auto index = change_index_(value);
		if (callback != nullptr)
			callback(*this, (index != static_cast<std::size_t>(-1)), index);
		return index;
	}

	thread_.queue.post([=]{
		auto index = change_index_(value);
		if (callback != nullptr)
			callback(*this, (index != static_cast<std::size_t>(-1)), index);
	}, thread::queue::send_priority, id_);

	return static_cast<std::size_t>(-1);
}

std::size_t winp::ui::object::get_index(const std::function<void(std::size_t)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_index_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_index_()); }, thread::queue::send_priority, id_);
		return static_cast<std::size_t>(-1);
	}

	return thread_.queue.execute([this]{ return get_index_(); }, thread::queue::send_priority, id_);
}

bool winp::ui::object::set_previous_sibling(object *target, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto value = set_previous_sibling_(target);
		if (callback != nullptr)
			callback(*this, value);
		return value;
	}

	thread_.queue.post([=]{
		auto value = set_previous_sibling_(target);
		if (callback != nullptr)
			callback(*this, value);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::ui::object *winp::ui::object::get_previous_sibling(const std::function<void(object *)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_previous_sibling_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_previous_sibling_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_previous_sibling_(); }, thread::queue::send_priority, id_);
}

bool winp::ui::object::set_next_sibling(object *target, const std::function<void(object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto value = set_next_sibling_(target);
		if (callback != nullptr)
			callback(*this, value);
		return value;
	}

	thread_.queue.post([=]{
		auto value = set_next_sibling_(target);
		if (callback != nullptr)
			callback(*this, value);
	}, thread::queue::send_priority, id_);

	return true;
}

winp::ui::object *winp::ui::object::get_next_sibling(const std::function<void(object *)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_next_sibling_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_next_sibling_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_next_sibling_(); }, thread::queue::send_priority, id_);
}

winp::utility::dynamic_list<winp::ui::tree, winp::ui::object> winp::ui::object::get_ancestors() const{
	return ancestor_list_;
}

winp::utility::dynamic_list<winp::ui::object, winp::ui::object> winp::ui::object::get_siblings() const{
	return sibling_list_;
}

const wchar_t *winp::ui::object::get_theme_name(const std::function<void(const wchar_t *)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_theme_name_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_theme_name_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_theme_name_(); }, thread::queue::send_priority, id_);
}

LRESULT winp::ui::object::send_message(UINT msg, const std::function<void(LRESULT)> &callback){
	return do_send_message_(msg, 0, 0, callback);
}

bool winp::ui::object::post_message(UINT msg, const std::function<void(bool)> &callback){
	return do_post_message_(msg, 0, 0, callback);
}

void winp::ui::object::init_(){
	ancestor_list_.init_([this](){//begin
		if (thread_.is_thread_context())
			return get_parent_();
		return thread_.queue.execute([this]() -> tree *{ return get_parent_(); }, thread::queue::send_priority, id_);
	}, [this](tree *current){//next
		if (current->thread_.is_thread_context())
			return current->get_parent_();
		return thread_.queue.execute([&]() -> tree *{ return current->get_parent_(); }, thread::queue::send_priority, id_);
	});

	sibling_list_.init_([this]() -> object *{//begin
		if (thread_.is_thread_context()){
			auto parent = get_parent_();
			if (parent == nullptr)
				return nullptr;

			return ((get_index_() == 0u) ? parent->get_child_at_(1) : parent->get_child_at_(0));
		}

		return thread_.queue.execute([this]() -> object *{
			auto parent = get_parent_();
			if (parent == nullptr)
				return nullptr;

			return ((get_index_() == 0u) ? parent->get_child_at_(1) : parent->get_child_at_(0));
		}, thread::queue::send_priority, id_);
	}, [this](object *current) -> object *{//next
		if (current->thread_.is_thread_context()){
			auto parent = get_parent_();
			if (parent == nullptr || current->get_parent_() != parent)
				return nullptr;

			std::size_t index = current->get_index_();
			auto value = parent_->get_child_at_(index + 1u);

			return ((value == this) ? parent_->get_child_at_(index + 2u) : value);
		}

		return thread_.queue.execute([&]() -> object *{
			auto parent = get_parent_();
			if (parent == nullptr || current->get_parent_() != parent)
				return nullptr;

			std::size_t index = current->get_index_();
			auto value = parent_->get_child_at_(index + 1u);

			return ((value == this) ? parent_->get_child_at_(index + 2u) : value);
		}, thread::queue::send_priority, id_);
	});
}

void winp::ui::object::destruct_(){
	item::destruct_();
	remove_parent_();
}

bool winp::ui::object::create_(){
	return true;
}

bool winp::ui::object::destroy_(){
	return true;
}

void winp::ui::object::set_handle_(HANDLE value){
	handle_ = value;
}

HANDLE winp::ui::object::get_handle_() const{
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

	if ((index = value->insert_child_(*this, index)) == static_cast<std::size_t>(-1))
		return static_cast<std::size_t>(-1);//Failed to insert into parent

	parent_changed_(previous_parent, previous_index);
	index_changed_(previous_parent, previous_index);

	dispatch_message_(WINP_WM_PARENT_CHANGED, reinterpret_cast<WPARAM>(previous_parent), static_cast<LPARAM>(previous_index), false);
	dispatch_message_(WINP_WM_INDEX_CHANGED, reinterpret_cast<WPARAM>(previous_parent), static_cast<LPARAM>(previous_index), false);

	return index;
}

bool winp::ui::object::remove_parent_(){
	auto previous_parent = get_parent_();
	if (previous_parent == nullptr)
		return true;

	if (!validate_parent_change_(nullptr, static_cast<std::size_t>(-1)))
		return false;

	auto previous_index = get_index_();
	if (!previous_parent->remove_child_(*this))
		return false;

	parent_changed_(previous_parent, previous_index);
	index_changed_(previous_parent, previous_index);

	dispatch_message_(WINP_WM_PARENT_CHANGED, reinterpret_cast<WPARAM>(previous_parent), static_cast<LPARAM>(previous_index), false);
	dispatch_message_(WINP_WM_INDEX_CHANGED, reinterpret_cast<WPARAM>(previous_parent), static_cast<LPARAM>(previous_index), false);

	if (previous_parent != nullptr){
		previous_parent->child_removed_(*this, previous_index);
		previous_parent->dispatch_message_(WINP_WM_CHILD_REMOVED, reinterpret_cast<WPARAM>(this), previous_index, false);
	}

	return true;
}

void winp::ui::object::parent_changing_(){}

void winp::ui::object::parent_changed_(tree *previous_parent, std::size_t previous_index){
	dispatch_message_(WINP_WM_PARENT_CHANGED, reinterpret_cast<WPARAM>(previous_parent), static_cast<LPARAM>(previous_index));
}

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
		if (value != static_cast<std::size_t>(-1) && value != previous_index){
			index_changed_(get_parent_(), previous_index);
			dispatch_message_(WINP_WM_INDEX_CHANGED, reinterpret_cast<WPARAM>(parent_), static_cast<LPARAM>(previous_index), false);
		}
	}
	else
		index_ = value;

	return value;
}

void winp::ui::object::index_changing_(){}

void winp::ui::object::index_changed_(tree *previous_parent, std::size_t previous_index){
	dispatch_message_(WINP_WM_INDEX_CHANGED, reinterpret_cast<WPARAM>(previous_parent), static_cast<LPARAM>(previous_index));
}

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

const wchar_t *winp::ui::object::get_theme_name_() const{
	return nullptr;
}

LRESULT winp::ui::object::do_send_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(LRESULT)> &callback){
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(send_message_(msg, wparam, lparam)); }, thread::queue::send_priority, id_);
		return 0;
	}

	return thread_.queue.execute([=]{ return send_message_(msg, wparam, lparam); }, thread::queue::send_priority, id_);
}

LRESULT winp::ui::object::send_message_(UINT msg, WPARAM wparam, LPARAM lparam){
	auto handle = get_handle_();
	if (handle != nullptr && IsWindow(static_cast<HWND>(handle)) != FALSE)
		return SendMessageW(static_cast<HWND>(handle), msg, wparam, lparam);

	return dispatch_message_(msg, wparam, lparam);
}

bool winp::ui::object::do_post_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(bool)> &callback){
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(post_message_(msg, wparam, lparam)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([=]{ return post_message_(msg, wparam, lparam); }, thread::queue::send_priority, id_);
}

bool winp::ui::object::post_message_(UINT msg, WPARAM wparam, LPARAM lparam){
	auto handle = get_handle_();
	if (handle != nullptr && IsWindow(static_cast<HWND>(handle)) != FALSE)
		return (PostMessageW(static_cast<HWND>(handle), msg, wparam, lparam) != FALSE);

	dispatch_message_(msg, wparam, lparam, false);
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

LRESULT winp::ui::object::dispatch_message_(UINT msg, WPARAM wparam, LPARAM lparam, bool call_default){
	return find_dispatcher_(msg)->dispatch_(*this, MSG{ static_cast<HWND>(get_handle_()), msg, wparam, lparam }, call_default);
}

winp::ui::tree *winp::ui::object::get_parent_of_(const object &target){
	return target.get_parent_();
}
