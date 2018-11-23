#include "../app/app_object.h"

winp::menu::check_item::check_item() = default;

winp::menu::check_item::check_item(thread::object &thread)
	: item(thread){}

winp::menu::check_item::check_item(ui::tree &parent)
	: item(parent){}

winp::menu::check_item::~check_item() = default;

bool winp::menu::check_item::check(const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = check_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = check_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::menu::check_item::uncheck(const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = uncheck_(false);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = uncheck_(false);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::menu::check_item::toggle_check(const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = toggle_check_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = toggle_check_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::menu::check_item::is_checked(const std::function<void(bool)> &callback) const{
	return has_state(MFS_CHECKED, callback);
}

bool winp::menu::check_item::set_checked_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_checked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_checked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

HBITMAP winp::menu::check_item::get_checked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_checked_bitmap_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_checked_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_checked_bitmap_(); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::check_item::set_unchecked_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_unchecked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_unchecked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

HBITMAP winp::menu::check_item::get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_unchecked_bitmap_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_unchecked_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_unchecked_bitmap_(); }, thread::queue::send_priority, id_).get();
}

HBITMAP winp::menu::check_item::get_checked_bitmap_() const{
	return checked_bitmap_;
}

HBITMAP winp::menu::check_item::get_unchecked_bitmap_() const{
	return unchecked_bitmap_;
}

bool winp::menu::check_item::select_(){
	return toggle_check_();
}

bool winp::menu::check_item::check_(){
	if (has_state_(MFS_CHECKED))
		return true;

	auto parent = get_parent_();
	auto radio_group_parent = dynamic_cast<radio_group *>(parent);

	check_item *check_child;
	if (radio_group_parent != nullptr){
		for (auto child : parent->children_){
			if ((check_child = dynamic_cast<check_item *>(child)) != nullptr)
				check_child->uncheck_(true);
		}
	}

	set_state_(MFS_CHECKED);
	if (!is_created_)
		return true;

	if (!update_states_())
		return false;

	dispatch_message_(WINP_WM_MENU_CHECK, reinterpret_cast<WPARAM>(static_cast<item_component *>(this)), 0);
	return true;
}

bool winp::menu::check_item::uncheck_(bool force){
	if (!has_state_(MFS_CHECKED))
		return true;

	if (!force && dynamic_cast<radio_group *>(get_parent_()) != nullptr)
		return false;//Cannot remove check from a radio item

	remove_state_(MFS_CHECKED);
	if (!is_created_)
		return true;

	if (!update_states_())
		return false;

	dispatch_message_(WINP_WM_MENU_UNCHECK, reinterpret_cast<WPARAM>(static_cast<item_component *>(this)), 0);
	return true;
}

bool winp::menu::check_item::toggle_check_(){
	return (has_state_(MFS_CHECKED) ? uncheck_(false) : check_());
}

bool winp::menu::check_item::set_checked_bitmap_(HBITMAP value){
	checked_bitmap_ = value;
	return update_check_marks_();
}

bool winp::menu::check_item::set_unchecked_bitmap_(HBITMAP value){
	unchecked_bitmap_ = value;
	return update_check_marks_();
}