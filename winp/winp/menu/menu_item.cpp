#include "../app/app_object.h"

winp::menu::item::item() = default;

winp::menu::item::item(bool)
	: item_component(false){}

winp::menu::item::item(thread::object &thread)
	: item_component(thread){}

winp::menu::item::item(thread::object &thread, bool)
	: item_component(thread, false){}

winp::menu::item::item(ui::tree &parent)
	: item_component(parent){}

winp::menu::item::item(ui::tree &parent, bool)
	: item_component(parent, false){}

winp::menu::item::~item() = default;

winp::ui::surface *winp::menu::item::get_popup(const std::function<void(ui::surface *)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_popup_());
	}, callback != nullptr);
}

bool winp::menu::item::set_label(const std::wstring &value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_label_(value));
	});
}

const std::wstring *winp::menu::item::get_label(const std::function<void(const std::wstring &)> &callback) const{
	return execute_or_post_([=]{
		auto result = get_label_();
		if (callback != nullptr)
			callback(*result);
		return result;
	}, callback != nullptr);
}

bool winp::menu::item::set_shortcut(const std::wstring &value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_shortcut_(value));
	});
}

const std::wstring *winp::menu::item::get_shortcut(const std::function<void(const std::wstring &)> &callback) const{
	return execute_or_post_([=]{
		auto result = get_shortcut_();
		if (callback != nullptr)
			callback(*result);
		return result;
	}, callback != nullptr);
}

bool winp::menu::item::make_default(const std::function<void(thread::item &, bool)> &callback){
	return set_state(MFS_DEFAULT, callback);
}

bool winp::menu::item::is_default(const std::function<void(bool)> &callback) const{
	return has_state(MFS_DEFAULT, callback);
}

bool winp::menu::item::set_bitmap(HBITMAP value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_bitmap_(value));
	});
}

HBITMAP winp::menu::item::get_bitmap(const std::function<void(HBITMAP)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_bitmap_());
	}, callback != nullptr);
}

bool winp::menu::item::select(const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		unsigned int states = 0;
		return pass_value_to_callback_(callback, select_(nullptr, nullptr, true, states));
	});
}

bool winp::menu::item::handle_child_insert_event_(event::tree &e){
	return (dynamic_cast<menu::object *>(e.get_target()) != nullptr);
}

bool winp::menu::item::handle_child_remove_event_(event::tree &e){
	return (dynamic_cast<const menu::object *>(e.get_target()) != nullptr);
}

void winp::menu::item::handle_child_inserted_event_(event::tree &e){
	if (popup_ != nullptr)//Remove previous target
		erase_child_(*popup_);

	if ((popup_ = dynamic_cast<ui::surface *>(e.get_target()))->get_handle_() != nullptr)
		update_popup_();
}

void winp::menu::item::handle_child_removed_event_(event::tree &e){
	popup_ = nullptr;
	update_popup_();
}

winp::ui::surface *winp::menu::item::get_popup_() const{
	return popup_;
}

bool winp::menu::item::set_label_(const std::wstring &value){
	auto index = value.find(L'\t');
	if (index < value.size()){//Split
		label_.assign(value.data(), index);
		if ((index + 1u) < value.size())
			shortcut_.assign((value.begin() + index + 1u), value.end());
		else//No shortcut
			shortcut_.clear();
	}
	else
		label_ = value;

	return update_label_();
}

const std::wstring *winp::menu::item::get_label_() const{
	return &label_;
}

bool winp::menu::item::set_shortcut_(const std::wstring &value){
	shortcut_ = value;
	return update_label_();
}

const std::wstring *winp::menu::item::get_shortcut_() const{
	return &shortcut_;
}

bool winp::menu::item::set_bitmap_(HBITMAP value){
	bitmap_ = value;
	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_BITMAP,											//Flags
		0,														//Types
		0,														//States
		0,														//Id
		nullptr,												//Sub-menu
		nullptr,												//Checked bitmap
		nullptr,												//Unchecked bitmap
		0,														//Data
		nullptr,												//String
		0,														//String size
		bitmap_
	});
}

HBITMAP winp::menu::item::get_bitmap_() const{
	return bitmap_;
}

bool winp::menu::item::select_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states){
	return select_(WINP_WM_MENU_SELECT, target, info, prevent_default, states);
}

bool winp::menu::item::select_(UINT msg, ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states){
	thread_.surface_manager_.select_menu_item_(msg, *this, target, info, prevent_default, states);
	return ((states & event::object::state_type::default_prevented) == 0u);
}

bool winp::menu::item::update_popup_(){
	auto handle = ((popup_ == nullptr) ? nullptr : popup_->get_handle_());
	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_SUBMENU,											//Flags
		0,														//Types
		0,														//States
		0,														//Id
		static_cast<HMENU>(handle)
	});
}

bool winp::menu::item::update_label_(){
	auto value = ((label_.empty() || shortcut_.empty()) ? label_ : (label_ + L"\t" + shortcut_));
	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_STRING,											//Flags
		0,														//Types
		0,														//States
		0,														//Id
		nullptr,												//Sub-menu
		nullptr,												//Checked bitmap
		nullptr,												//Unchecked bitmap
		0,														//Data
		value.data(),
		static_cast<UINT>(value.size())
	});
}

bool winp::menu::item::update_check_marks_(){
	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_CHECKMARKS,										//Flags
		0,														//Types
		0,														//States
		0,														//Id
		nullptr,												//Sub-menu
		checked_bitmap_,
		unchecked_bitmap_
	});
}
