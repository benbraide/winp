#include "../app/app_object.h"

winp::menu::check_item::check_item() = default;

winp::menu::check_item::check_item(thread::object &thread)
	: item(thread){}

winp::menu::check_item::check_item(ui::tree &parent)
	: item(parent){}

winp::menu::check_item::~check_item() = default;

bool winp::menu::check_item::is_radio(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_radio_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_radio_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_radio_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::check_item::check(const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		unsigned int states = 0;
		auto result = check_(nullptr, nullptr, true, states);

		if (callback != nullptr)
			callback(*this, result);

		return result;
	}

	thread_.queue.post([=]{
		unsigned int states = 0;
		auto result = check_(nullptr, nullptr, true, states);

		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::menu::check_item::uncheck(const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		unsigned int states = 0;
		auto result = uncheck_(nullptr, nullptr, true, states, false);

		if (callback != nullptr)
			callback(*this, result);

		return result;
	}

	thread_.queue.post([=]{
		unsigned int states = 0;
		auto result = uncheck_(nullptr, nullptr, true, states, false);

		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::menu::check_item::toggle_check(const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		unsigned int states = 0;
		auto result = toggle_check_(nullptr, nullptr, true, states);

		if (callback != nullptr)
			callback(*this, result);

		return result;
	}

	thread_.queue.post([=]{
		unsigned int states = 0;
		auto result = toggle_check_(nullptr, nullptr, true, states);

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

	return thread_.queue.execute([this]{ return get_checked_bitmap_(); }, thread::queue::send_priority, id_);
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

	return thread_.queue.execute([this]{ return get_unchecked_bitmap_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::check_item::handle_child_insert_event_(event::tree &e){
	return false;
}

bool winp::menu::check_item::handle_child_remove_event_(event::tree &e){
	return false;
}

HBITMAP winp::menu::check_item::get_checked_bitmap_() const{
	return checked_bitmap_;
}

HBITMAP winp::menu::check_item::get_unchecked_bitmap_() const{
	return unchecked_bitmap_;
}

bool winp::menu::check_item::select_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states){
	return toggle_check_(target, info, prevent_default, states);
}

bool winp::menu::check_item::is_radio_() const{
	return (get_first_ancestor_of_<menu::radio_group, menu::object>() != nullptr);
}

bool winp::menu::check_item::check_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states){
	if (has_state_(MFS_CHECKED))
		return true;

	if (auto parent = get_parent_(); (parent != nullptr && is_radio_() && !uncheck_siblings_(*parent, target, true)) || !item::select_(WINP_WM_MENU_CHECK, target, info, prevent_default, states))
		return false;//Rejected

	return set_state_(MFS_CHECKED);
}

bool winp::menu::check_item::uncheck_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states, bool force){
	if (!has_state_(MFS_CHECKED))
		return true;

	if ((!force && is_radio_()) || !item::select_(WINP_WM_MENU_UNCHECK, target, info, prevent_default, states))
		return false;//Cannot remove check from a radio item OR rejected

	return remove_state_(MFS_CHECKED);
}

bool winp::menu::check_item::toggle_check_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states){
	return (has_state_(MFS_CHECKED) ? uncheck_(target, info, prevent_default, states, false) : check_(target, info, prevent_default, states));
}

bool winp::menu::check_item::uncheck_siblings_(ui::tree &parent, ui::surface *target, bool force){
	check_item *check_child;
	unsigned int states = 0;

	for (auto child : parent.children_){
		if (dynamic_cast<menu::tree *>(child) != nullptr && !uncheck_siblings_(*dynamic_cast<ui::tree *>(child), target, force))
			return false;

		if ((check_child = dynamic_cast<check_item *>(child)) != nullptr && !check_child->uncheck_(target, nullptr, true, states, force))
			return false;
	}

	return true;
}

bool winp::menu::check_item::set_checked_bitmap_(HBITMAP value){
	checked_bitmap_ = value;
	return update_check_marks_();
}

bool winp::menu::check_item::set_unchecked_bitmap_(HBITMAP value){
	unchecked_bitmap_ = value;
	return update_check_marks_();
}
