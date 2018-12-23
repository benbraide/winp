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

bool winp::ui::object::create(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, create_());
	});
}

bool winp::ui::object::destroy(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, destroy_());
	});
}

bool winp::ui::object::is_created(const std::function<void(bool)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, is_created_());
	}, callback != nullptr);
}

HANDLE winp::ui::object::get_handle(const std::function<void(HANDLE)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_handle_());
	}, callback != nullptr);
}

bool winp::ui::object::is_ancestor(const tree &target, const std::function<void(bool)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, is_ancestor_(target));
	}, callback != nullptr);
}

std::size_t winp::ui::object::set_parent(tree *value, const std::function<void(thread::item &, std::size_t)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, change_parent_(value));
	});
}

winp::ui::tree *winp::ui::object::get_parent(const std::function<void(tree *)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_parent_());
	}, callback != nullptr);
}

std::size_t winp::ui::object::set_index(std::size_t value, const std::function<void(thread::item &, std::size_t)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, change_index_(value));
	});
}

std::size_t winp::ui::object::get_index(const std::function<void(std::size_t)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_index_());
	}, callback != nullptr);
}

bool winp::ui::object::set_previous_sibling(object *target, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_previous_sibling_(target));
	});
}

winp::ui::object *winp::ui::object::get_previous_sibling(const std::function<void(object *)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_previous_sibling_());
	}, callback != nullptr);
}

bool winp::ui::object::set_next_sibling(object *target, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_next_sibling_(target));
	});
}

winp::ui::object *winp::ui::object::get_next_sibling(const std::function<void(object *)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_next_sibling_());
	}, callback != nullptr);
}

winp::utility::dynamic_list<winp::ui::tree, winp::ui::object> winp::ui::object::get_ancestors() const{
	return ancestor_list_;
}

winp::utility::dynamic_list<winp::ui::object, winp::ui::object> winp::ui::object::get_siblings() const{
	return sibling_list_;
}

const wchar_t *winp::ui::object::get_theme_name(const std::function<void(const wchar_t *)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_theme_name_());
	}, callback != nullptr);
}

LRESULT winp::ui::object::send_message(UINT msg, const std::function<void(LRESULT)> &callback){
	return do_send_message_(msg, 0, 0, callback);
}

bool winp::ui::object::post_message(UINT msg, const std::function<void(bool)> &callback){
	return do_post_message_(msg, 0, 0, callback);
}

bool winp::ui::object::remove_hook(unsigned int code, const std::function<void(thread::item &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = remove_hook_(code);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = remove_hook_(code);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return false;
}

bool winp::ui::object::has_hook(unsigned int code, const std::function<void(bool)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, has_hook_(code));
	}, callback != nullptr);
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

bool winp::ui::object::is_created_() const{
	return false;
}

void winp::ui::object::add_to_toplevel_(bool update){}

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

bool winp::ui::object::is_ancestor_(const tree &target) const{
	auto parent = get_parent_();
	return (parent != nullptr && (&target == parent || parent->is_ancestor_(target)));
}

void winp::ui::object::set_parent_(tree *value){
	parent_ = value;
}

winp::ui::tree *winp::ui::object::get_parent_() const{
	return parent_;
}

std::size_t winp::ui::object::change_parent_(tree *value, std::size_t index){
	auto previous_parent = get_parent_();
	if (value == previous_parent)//Same parent
		return change_index_(index);

	if (value == nullptr){
		remove_parent_();
		return static_cast<std::size_t>(-1);
	}

	tree_change_info info{
		this,
		value,
		index
	};

	auto previous_index = get_index_();
	if (value->thread_.id_ != thread_.id_ || dispatch_message_(WINP_WM_PARENT_CHANGING, reinterpret_cast<WPARAM>(&info), 0) != 0)
		return static_cast<std::size_t>(-1);

	if (previous_parent != nullptr && !previous_parent->remove_child_(*this))
		return static_cast<std::size_t>(-1);//Failed to remove from parent

	if ((index = value->insert_child_(*this, index)) == static_cast<std::size_t>(-1))
		return static_cast<std::size_t>(-1);//Failed to insert into parent

	info.parent = previous_parent;
	info.index = previous_index;

	dispatch_message_(WINP_WM_PARENT_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);
	dispatch_message_(WINP_WM_INDEX_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);

	if (auto handle = ((dynamic_cast<ui::window_surface *>(this) == nullptr) ? nullptr : static_cast<HWND>(get_handle_())); handle != nullptr){
		add_to_toplevel_(true);
		if (IsWindow(handle)){
			SetWindowLongPtrW(handle, GWL_STYLE, ((static_cast<UINT>(GetWindowLongPtrW(handle, GWL_STYLE)) | WS_CHILD) & ~WS_POPUP));
			auto window_parent = value->get_first_ancestor_of_<ui::window_surface>();
			if (auto parent_handle = ((window_parent == nullptr) ? nullptr : static_cast<HWND>(window_parent->get_handle_())); parent_handle != nullptr)
				SetParent(handle, parent_handle);
		}
	}
	else if (auto non_window_self = dynamic_cast<non_window::child *>(this); non_window_self != nullptr)
		non_window_self->redraw_(RECT{});

	return index;
}

bool winp::ui::object::remove_parent_(){
	auto previous_parent = get_parent_();
	if (previous_parent == nullptr)
		return true;

	tree_change_info info{
		this,
		nullptr,
		static_cast<std::size_t>(-1)
	};

	if (dispatch_message_(WINP_WM_PARENT_CHANGING, reinterpret_cast<WPARAM>(&info), 0) != 0)
		return false;

	if (auto non_window_self = dynamic_cast<non_window::child *>(this); non_window_self != nullptr)
		non_window_self->redraw_(RECT{});

	auto previous_index = get_index_();
	if (!previous_parent->remove_child_(*this))
		return false;

	info.parent = previous_parent;
	info.index = previous_index;

	dispatch_message_(WINP_WM_PARENT_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);
	dispatch_message_(WINP_WM_INDEX_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);

	if (auto handle = ((dynamic_cast<ui::window_surface *>(this) == nullptr) ? nullptr : static_cast<HWND>(get_handle_())); handle != nullptr){
		add_to_toplevel_(true);
		if (IsWindow(handle)){
			SetParent(handle, nullptr);
			SetWindowLongPtrW(handle, GWL_STYLE, (static_cast<UINT>(GetWindowLongPtrW(handle, GWL_STYLE)) & ~WS_CHILD));
		}
	}

	return true;
}

std::size_t winp::ui::object::change_index_(std::size_t value){
	auto parent = get_parent_();
	if (parent == nullptr)
		return (index_ = value);

	auto previous_index = get_index_();
	if (value == previous_index)
		return value;

	tree_change_info info{
		this,
		parent,
		value
	};

	if (dispatch_message_(WINP_WM_INDEX_CHANGING, reinterpret_cast<WPARAM>(&info), 0) != 0)
		return static_cast<std::size_t>(-1);

	info.index = previous_index;
	if (parent != nullptr && (value = parent->change_child_index_(*this, value)) != static_cast<std::size_t>(-1))
		dispatch_message_(WINP_WM_INDEX_CHANGED, reinterpret_cast<WPARAM>(&info), 0, false);
	else if (parent == nullptr)
		index_ = value;

	return value;
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

bool winp::ui::object::remove_hook_(unsigned int code){
	if (!hook_map_.empty())
		return false;

	if ((code & ui::hook::parent_size_change_hook_code) != 0u)
		hook_map_.erase(ui::hook::parent_size_change_hook_code);

	if ((code & ui::hook::child_size_change_hook_code) != 0u)
		hook_map_.erase(ui::hook::child_size_change_hook_code);

	return true;
}

bool winp::ui::object::has_hook_(unsigned int code) const{
	return (find_hook_(code) != nullptr);
}

const winp::ui::object::hook_list_type *winp::ui::object::find_hook_(unsigned int code) const{
	auto it = hook_map_.find(code);
	return ((it == hook_map_.end()) ? nullptr : &it->second);
}

void winp::ui::object::call_hook_(unsigned int code){
	const hook_list_type *hook_list = nullptr;
	if ((code & ui::hook::parent_size_change_hook_code) != 0u && (hook_list = find_hook_(ui::hook::parent_size_change_hook_code)) != nullptr){
		called_hook_ |= ui::hook::parent_size_change_hook_code;
		for (auto &hook : *hook_list)
			hook.second->handle_hook_callback(ui::hook::parent_size_change_hook_code);
		called_hook_ &= ~ui::hook::parent_size_change_hook_code;
	}

	if ((code & ui::hook::child_size_change_hook_code) != 0u && (hook_list = find_hook_(ui::hook::child_size_change_hook_code)) != nullptr){
		called_hook_ |= ui::hook::child_size_change_hook_code;
		for (auto &hook : *hook_list)
			hook.second->handle_hook_callback(ui::hook::child_size_change_hook_code);
		called_hook_ &= ~ui::hook::child_size_change_hook_code;
	}
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
	return find_dispatcher_(msg)->dispatch_(*this, MSG{ static_cast<HWND>(get_handle_()), msg, wparam, lparam }, call_default, nullptr);
}

winp::ui::tree *winp::ui::object::get_parent_of_(const object &target){
	return target.get_parent_();
}
