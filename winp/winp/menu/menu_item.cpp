#include "../app/app_object.h"
#include "menu_item.h"

winp::menu::item::item(){
	local_id_ = thread_.menu_random_generator_(static_cast<WORD>(1), std::numeric_limits<WORD>::max());
}

winp::menu::item::item(thread::object &thread)
	: surface(thread){
	local_id_ = thread_.menu_random_generator_(static_cast<WORD>(1), std::numeric_limits<WORD>::max());
}

winp::menu::item::item(ui::tree &parent)
	: surface(parent.get_thread()){
	local_id_ = thread_.menu_random_generator_(static_cast<WORD>(1), std::numeric_limits<WORD>::max());
	set_parent_(&parent);
}

winp::menu::item::~item(){
	thread_.queue.add([=]{
		remove_parent_();
	}, thread::queue::send_priority, id_).get();
}

std::size_t winp::menu::item::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_index_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([this]{ return get_absolute_index_(); }, thread::queue::send_priority, id_).get();
}

WORD winp::menu::item::get_local_id() const{
	return local_id_;
}

winp::ui::surface *winp::menu::item::get_target(const std::function<void(ui::surface *)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_target_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_target_(); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::set_label(const std::wstring &value, const std::function<void(item &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_label_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

std::wstring winp::menu::item::get_label(const std::function<void(const std::wstring &)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_label_()); }, thread::queue::send_priority, id_);
		return L"";
	}

	return thread_.queue.add([this]{ return get_label_(); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::set_shortcut(const std::wstring &value, const std::function<void(item &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_shortcut_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

std::wstring winp::menu::item::get_shortcut(const std::function<void(const std::wstring &)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_shortcut_()); }, thread::queue::send_priority, id_);
		return L"";
	}

	return thread_.queue.add([this]{ return get_shortcut_(); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::set_state(UINT value, const std::function<void(item &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_state_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

void winp::menu::item::remove_state(UINT value, const std::function<void(item &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = remove_state_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

UINT winp::menu::item::get_states(const std::function<void(UINT)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_states_()); }, thread::queue::send_priority, id_);
		return 0u;
	}

	return thread_.queue.add([this]{ return get_states_(); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::item::has_state(UINT value, const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(has_state_(value)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([=]{ return has_state_(value); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::item::has_states(UINT value, const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(has_states_(value)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([=]{ return has_states_(value); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::enable(const std::function<void(item &, bool)> &callback){
	remove_state(MFS_DISABLED, callback);
}

void winp::menu::item::disable(const std::function<void(item &, bool)> &callback){
	set_state(MFS_DISABLED, callback);
}

bool winp::menu::item::is_disabled(const std::function<void(bool)> &callback) const{
	return has_state(MFS_DISABLED, callback);
}

void winp::menu::item::make_default(const std::function<void(item &, bool)> &callback){
	set_state(MFS_DEFAULT, callback);
}

bool winp::menu::item::is_default(const std::function<void(bool)> &callback) const{
	return has_state(MFS_DEFAULT, callback);
}

bool winp::menu::item::is_owner_drawn(const std::function<void(bool)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(has_type_(MFT_OWNERDRAW)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.add([this]{ return has_type_(MFT_OWNERDRAW); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::set_bitmap(HBITMAP value, const std::function<void(item &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

HBITMAP winp::menu::item::get_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_bitmap_(); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::set_checked_bitmap(HBITMAP value, const std::function<void(item &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_checked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

HBITMAP winp::menu::item::get_checked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_checked_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_checked_bitmap_(); }, thread::queue::send_priority, id_).get();
}

void winp::menu::item::set_unchecked_bitmap(HBITMAP value, const std::function<void(item &, bool)> &callback){
	thread_.queue.post([=]{
		auto result = set_unchecked_bitmap_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);
}

HBITMAP winp::menu::item::get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_unchecked_bitmap_()); }, thread::queue::send_priority, id_);
		return nullptr;
	}

	return thread_.queue.add([this]{ return get_unchecked_bitmap_(); }, thread::queue::send_priority, id_).get();
}

bool winp::menu::item::create_(){
	auto parent = get_parent_();
	if (parent == nullptr)
		return false;;

	auto handle = static_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr)//Parent not created
		return true;

	UINT mask = (MIIM_ID | MIIM_DATA);
	if (!label_.empty())//Label set
		mask |= MIIM_STRING;

	if (types_ != 0u)//Type set
		mask |= MIIM_FTYPE;

	if (states_ != 0u)//States set
		mask |= MIIM_STATE;

	if (bitmap_ != nullptr)//Bitmap set
		mask |= MIIM_BITMAP;

	if (checked_bitmap_ != nullptr || unchecked_bitmap_ != nullptr)
		mask |= MIIM_CHECKMARKS;//Bitmaps set

	auto popup = dynamic_cast<menu::object *>(get_target_());
	if (popup != nullptr)//Has sub menu
		mask |= MIIM_SUBMENU;

	auto label = ((label_.empty() || shortcut_.empty()) ? label_ : (label_ + L"\t" + shortcut_));
	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		mask,
		get_types_(),
		states_,
		local_id_,
		((popup == nullptr) ? nullptr : static_cast<HMENU>(popup->get_handle_())),
		checked_bitmap_,
		unchecked_bitmap_,
		reinterpret_cast<ULONG_PTR>(this),
		label.data(),
		static_cast<UINT>(label.size()),
		bitmap_
	};

	if (InsertMenuItemW(handle, static_cast<UINT>(get_absolute_index_()), TRUE, &info) == FALSE)
		return false;

	auto menu_parent = dynamic_cast<menu::object *>(parent);
	if (menu_parent != nullptr)
		menu_parent->redraw_();

	return true;
}

bool winp::menu::item::destroy_(){
	return remove_parent_();
}

bool winp::menu::item::validate_parent_change_(ui::tree *value, std::size_t index) const{
	return (surface::validate_parent_change_(value, index) && (value == nullptr || dynamic_cast<menu::tree *>(value) != nullptr));
}

void winp::menu::item::parent_changed_(ui::tree *previous_parent, std::size_t previous_index){
	if (previous_parent != nullptr)//Remove from previous parent
		remove_from_parent_(*previous_parent);

	create_();
	surface::parent_changed_(previous_parent, previous_index);
}

void winp::menu::item::index_changed_(ui::tree *previous_parent, std::size_t previous_index){
	auto parent = get_parent_();
	if (previous_parent == parent && parent != nullptr && remove_from_parent_(*parent))
		create_();

	surface::index_changed_(previous_parent, previous_index);
}

bool winp::menu::item::validate_child_insert_(const ui::object &child, std::size_t index) const{
	return (dynamic_cast<const menu::object *>(&child) != nullptr);
}

void winp::menu::item::child_inserted_(ui::object &child, tree *previous_parent, std::size_t previous_index){
	if (target_ != nullptr)//Remove previous target
		erase_child_(*target_);

	target_ = dynamic_cast<ui::surface *>(&child);
	surface::child_inserted_(child, previous_parent, previous_index);
}

bool winp::menu::item::validate_child_remove_(const ui::object &child) const{
	return (dynamic_cast<const ui::surface *>(&child) == target_);
}

void winp::menu::item::child_removed_(ui::object &child, std::size_t previous_index){
	target_ = nullptr;
	surface::child_removed_(child, previous_index);
}

std::size_t winp::menu::item::get_count_() const{
	return 1u;
}

std::size_t winp::menu::item::get_absolute_index_() const{
	auto parent = dynamic_cast<menu::tree *>(get_parent_());
	return ((parent == nullptr) ? get_index_() : parent->get_absolute_index_of_(*this));
}

bool winp::menu::item::remove_from_parent_(ui::tree &parent){
	auto handle = static_cast<HMENU>(parent.get_handle_());
	if (handle == nullptr)//Parent not created
		return true;

	auto result = RemoveMenu(handle, local_id_, MF_BYCOMMAND);
	auto menu_parent = dynamic_cast<menu::object *>(&parent);
	if (menu_parent != nullptr)
		menu_parent->redraw_();

	return (result != FALSE);
}

winp::ui::surface *winp::menu::item::get_target_() const{
	return target_;
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

const std::wstring &winp::menu::item::get_label_() const{
	return label_;
}

bool winp::menu::item::set_shortcut_(const std::wstring &value){
	shortcut_ = value;
	return update_label_();
}

const std::wstring &winp::menu::item::get_shortcut_() const{
	return shortcut_;
}

bool winp::menu::item::set_state_(UINT value){
	auto old_states = states_;
	states_ |= (value & ~get_filtered_states_());
	return ((old_states == states_) ? true : update_state_());
}

bool winp::menu::item::remove_state_(UINT value){
	auto old_states = states_;
	states_ &= ~(value & ~get_filtered_states_());
	return ((old_states == states_) ? true : update_state_());
}

UINT winp::menu::item::get_states_() const{
	return (states_ | get_persistent_states_());
}

UINT winp::menu::item::get_persistent_states_() const{
	return 0u;
}

UINT winp::menu::item::get_filtered_states_() const{
	return 0u;
}

bool winp::menu::item::has_state_(UINT value) const{
	return ((get_states_() & value) != 0u);
}

bool winp::menu::item::has_states_(UINT value) const{
	return ((get_states_() & value) == value);
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

UINT winp::menu::item::get_types_() const{
	return types_;
}

bool winp::menu::item::has_type_(UINT value) const{
	return ((get_types_() & value) == value);
}

bool winp::menu::item::update_(const MENUITEMINFOW &info){
	auto parent = get_parent_();
	if (parent == nullptr)
		return true;

	auto handle = static_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr)//Parent not created
		return true;

	return (SetMenuItemInfoW(handle, local_id_, FALSE, &info) != FALSE);
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

bool winp::menu::item::update_state_(){
	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_STATE,												//Flags
		0,														//Types
		states_
	});
}

bool winp::menu::item::update_type_(){
	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_FTYPE,												//Flags
		get_types_()
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
