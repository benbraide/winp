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
	if (thread_.is_thread_context()){
		auto result = get_popup_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_popup_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_popup_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::item::set_label(const std::wstring &value, const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_label_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_label_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

const std::wstring *winp::menu::item::get_label(const std::function<void(const std::wstring &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_label_();
		if (callback != nullptr)
			callback(*result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(*get_label_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_label_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::item::set_shortcut(const std::wstring &value, const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_shortcut_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_shortcut_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

const std::wstring *winp::menu::item::get_shortcut(const std::function<void(const std::wstring &)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_shortcut_();
		if (callback != nullptr)
			callback(*result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(*get_shortcut_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_shortcut_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::item::make_default(const std::function<void(item_component &, bool)> &callback){
	return set_state(MFS_DEFAULT, callback);
}

bool winp::menu::item::is_default(const std::function<void(bool)> &callback) const{
	return has_state(MFS_DEFAULT, callback);
}

bool winp::menu::item::set_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

HBITMAP winp::menu::item::get_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_bitmap_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.execute([this]{ return get_bitmap_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::item::select(const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = select_();
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = select_();
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
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

bool winp::menu::item::select_(){
	if (!is_created_)
		return false;

	dispatch_message_(WINP_WM_MENU_SELECT, reinterpret_cast<WPARAM>(static_cast<item_component *>(this)), 0);
	return true;
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
