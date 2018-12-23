#include "../app/app_object.h"

winp::menu::check_item::check_item() = default;

winp::menu::check_item::check_item(thread::object &thread)
	: item(thread){}

winp::menu::check_item::check_item(ui::tree &parent)
	: item(parent){}

winp::menu::check_item::~check_item() = default;

bool winp::menu::check_item::is_radio(const std::function<void(bool)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, is_radio_());
	}, callback != nullptr);
}

bool winp::menu::check_item::check(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		unsigned int states = 0;
		return pass_value_to_callback_(callback, check_(nullptr, nullptr, true, states));
	});
}

bool winp::menu::check_item::uncheck(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		unsigned int states = 0;
		return pass_value_to_callback_(callback, uncheck_(nullptr, nullptr, true, states, false));
	});
}

bool winp::menu::check_item::toggle_check(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		unsigned int states = 0;
		return pass_value_to_callback_(callback, toggle_check_(nullptr, nullptr, true, states));
	});
}

bool winp::menu::check_item::is_checked(const std::function<void(bool)> &callback) const{
	return has_state(MFS_CHECKED, callback);
}

bool winp::menu::check_item::set_checked_bitmap(HBITMAP value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_checked_bitmap_(value));
	});
}

HBITMAP winp::menu::check_item::get_checked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_checked_bitmap_());
	}, callback != nullptr);
}

bool winp::menu::check_item::set_unchecked_bitmap(HBITMAP value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_unchecked_bitmap_(value));
	});
}

HBITMAP winp::menu::check_item::get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_unchecked_bitmap_());
	}, callback != nullptr);
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
