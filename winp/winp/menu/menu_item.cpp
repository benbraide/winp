#include "../app/app_object.h"

winp::menu::item::item() = default;

winp::menu::item::item(thread::object &thread)
	: item_component(thread){}

winp::menu::item::item(ui::tree &parent)
	: item_component(parent){}

winp::menu::item::item(ui::tree &parent, bool)
	: item_component(parent){}

winp::menu::item::~item(){
	destruct_();
}

winp::ui::surface *winp::menu::item::get_popup(const std::function<void(ui::surface *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_popup_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_popup_(); }, thread::queue::send_priority, id_).get();
}

const std::wstring *winp::menu::item::get_label(const std::function<void(const std::wstring &)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(*get_label_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_label_(); }, thread::queue::send_priority, id_).get();
}

const std::wstring *winp::menu::item::get_shortcut(const std::function<void(const std::wstring &)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(*get_shortcut_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_shortcut_(); }, thread::queue::send_priority, id_).get();
}

HBITMAP winp::menu::item::get_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_bitmap_(); }, thread::queue::send_priority, id_).get();
}

HBITMAP winp::menu::item::get_checked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_checked_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_checked_bitmap_(); }, thread::queue::send_priority, id_).get();
}

HBITMAP winp::menu::item::get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_unchecked_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_unchecked_bitmap_(); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::set_label(const std::wstring &value, const std::function<void(item_component &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_label_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::menu::item::set_shortcut(const std::wstring &value, const std::function<void(item_component &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_shortcut_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::menu::item::make_default(const std::function<void(item_component &, bool)> &callback){
	set_state(MFS_DEFAULT, callback);
}

bool winp::menu::item::is_default(const std::function<void(bool)> &callback) const{
	return has_state(MFS_DEFAULT, callback);
}

void winp::menu::item::set_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::menu::item::set_checked_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_checked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::menu::item::set_unchecked_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_unchecked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

bool winp::menu::item::validate_child_insert_(const ui::object &child, std::size_t index) const{
	return (item_component::validate_child_insert_(child, index) && dynamic_cast<const menu::object *>(&child) != nullptr);
}

void winp::menu::item::child_inserted_(ui::object &child, tree *previous_parent, std::size_t previous_index){
	if (popup_ != nullptr)//Remove previous target
		erase_child_(*popup_);

	popup_ = dynamic_cast<ui::surface *>(&child);
	item_component::child_inserted_(child, previous_parent, previous_index);
}

bool winp::menu::item::validate_child_remove_(const ui::object &child) const{
	return (item_component::validate_child_remove_(child) && dynamic_cast<const ui::surface *>(&child) == popup_);
}

void winp::menu::item::child_removed_(ui::object &child, std::size_t previous_index){
	popup_ = nullptr;
	item_component::child_removed_(child, previous_index);
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

bool winp::menu::item::set_checked_bitmap_(HBITMAP value){
	checked_bitmap_ = value;
	return update_check_marks_();
}

HBITMAP winp::menu::item::get_checked_bitmap_() const{
	return checked_bitmap_;
}

bool winp::menu::item::set_unchecked_bitmap_(HBITMAP value){
	unchecked_bitmap_ = value;
	return update_check_marks_();
}

HBITMAP winp::menu::item::get_unchecked_bitmap_() const{
	return unchecked_bitmap_;
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
