#include "../app/app_object.h"

winp::menu::item_component::item_component(){
	generate_id_();
}

winp::menu::item_component::item_component(bool){}

winp::menu::item_component::item_component(thread::object &thread)
	: surface(thread){
	generate_id_();
}

winp::menu::item_component::item_component(thread::object &thread, bool)
	: surface(thread){}

winp::menu::item_component::item_component(ui::tree &parent)
	: surface(parent.get_thread()){
	generate_id_();
	change_parent_(&parent);
}

winp::menu::item_component::item_component(ui::tree &parent, bool)
	: surface(parent.get_thread()){
	change_parent_(&parent);
}

winp::menu::item_component::~item_component(){
	destruct();
}

std::size_t winp::menu::item_component::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_absolute_index_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_absolute_index_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return get_absolute_index_(); }, thread::queue::send_priority, id_);
}

UINT winp::menu::item_component::get_local_id(const std::function<void(UINT)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_local_id_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_local_id_()); }, thread::queue::send_priority, id_);
		return 0u;
	}

	return thread_.queue.execute([this]{ return get_local_id_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::item_component::set_state(UINT value, const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_state_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_state_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

bool winp::menu::item_component::remove_state(UINT value, const std::function<void(item_component &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = remove_state_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = remove_state_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

UINT winp::menu::item_component::get_states(const std::function<void(UINT)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_states_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_states_()); }, thread::queue::send_priority, id_);
		return 0u;
	}

	return thread_.queue.execute([this]{ return get_states_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::item_component::has_state(UINT value, const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = has_state_(value);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(has_state_(value)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([=]{ return has_state_(value); }, thread::queue::send_priority, id_);
}

bool winp::menu::item_component::has_states(UINT value, const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = has_states_(value);
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(has_states_(value)); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([=]{ return has_states_(value); }, thread::queue::send_priority, id_);
}

bool winp::menu::item_component::enable(const std::function<void(item_component &, bool)> &callback){
	return remove_state(MFS_DISABLED, callback);
}

bool winp::menu::item_component::disable(const std::function<void(item_component &, bool)> &callback){
	return set_state(MFS_DISABLED, callback);
}

bool winp::menu::item_component::is_disabled(const std::function<void(bool)> &callback) const{
	return has_state(MFS_DISABLED, callback);
}

bool winp::menu::item_component::is_owner_drawn(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_owner_drawn_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_owner_drawn_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_owner_drawn_(); }, thread::queue::send_priority, id_);
}

bool winp::menu::item_component::is_popup_item(const std::function<void(bool)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = is_popup_item_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(is_popup_item_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return is_popup_item_(); }, thread::queue::send_priority, id_);
}

void winp::menu::item_component::event_handlers_count_changed_(event::manager_base &e, std::size_t previous_count, std::size_t current_count){
	if (&e == &draw_item_event && (previous_count == 0u || current_count == 0u))
		update_types_();
}

void winp::menu::item_component::destruct_(){
	destroy_();
	if (local_id_ != 0u && !thread_.surface_manager_.id_map_.empty()){
		thread_.surface_manager_.id_map_.erase(local_id_);
		local_id_ = 0u;
	}
	surface::destruct_();
}

bool winp::menu::item_component::create_(){
	if (is_created_)
		return true;

	auto parent = get_parent_();
	if (parent == nullptr)
		return false;

	auto handle = static_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr || IsMenu(handle) == FALSE)//Parent not created
		return true;

	UINT mask = 0u;
	if (local_id_ != 0u)
		mask |= MIIM_ID;

	auto plabel = get_label_();
	auto pshortcut = get_shortcut_();

	if (plabel != nullptr && !plabel->empty())//Label set
		mask |= MIIM_STRING;

	auto types = get_types_();
	if (types != 0u)//Type set
		mask |= MIIM_FTYPE;

	auto states = get_states_();
	if (states != 0u)//States set
		mask |= MIIM_STATE;

	auto bitmap = get_bitmap_();
	if (bitmap != nullptr)//Bitmap set
		mask |= MIIM_BITMAP;

	auto checked_bitmap = get_checked_bitmap_(), unchecked_bitmap = get_unchecked_bitmap_();
	if (checked_bitmap != nullptr || unchecked_bitmap != nullptr)
		mask |= MIIM_CHECKMARKS;//Bitmaps set

	auto popup = dynamic_cast<menu::object *>(get_popup_());
	if (popup != nullptr)//Has sub menu
		mask |= MIIM_SUBMENU;

	std::wstring label;
	if (plabel != nullptr)
		label = ((plabel->empty() || pshortcut == nullptr || pshortcut->empty()) ? *plabel : (*plabel + L"\t" + *pshortcut));

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		mask,
		types,
		states,
		local_id_,
		((popup == nullptr) ? nullptr : static_cast<HMENU>(popup->get_handle_())),
		checked_bitmap,
		unchecked_bitmap,
		0u,
		label.data(),
		static_cast<UINT>(label.size()),
		bitmap
	};

	if (InsertMenuItemW(handle, static_cast<UINT>(get_absolute_index_()), TRUE, &info) == FALSE)
		return false;

	auto menu_parent = dynamic_cast<menu::object *>(parent);
	if (menu_parent != nullptr)
		menu_parent->redraw_();

	dispatch_message_(WM_NCCREATE, 0, 0);
	return (is_created_ = true);
}

bool winp::menu::item_component::destroy_(){
	if (!is_created_)
		return true;

	is_created_ = false;
	auto parent = get_parent_();
	if (parent == nullptr)
		return true;

	auto handle = static_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr || IsMenu(handle) == FALSE)//Parent not created
		return true;

	if (((local_id_ == 0u) ? RemoveMenu(handle, static_cast<UINT>(get_absolute_index_()), MF_BYPOSITION) : RemoveMenu(handle, local_id_, MF_BYCOMMAND)) == FALSE)
		return false;

	auto menu_parent = dynamic_cast<menu::object *>(parent);
	if (menu_parent != nullptr)
		menu_parent->redraw_();

	dispatch_message_(WM_NCDESTROY, 0, 0);
	return true;
}

const wchar_t *winp::menu::item_component::get_theme_name_() const{
	return L"MENU";
}

std::size_t winp::menu::item_component::get_count_() const{
	return 1u;
}

bool winp::menu::item_component::handle_parent_change_event_(event::tree &e){
	if (e.get_attached_parent() != nullptr && dynamic_cast<menu::tree *>(e.get_attached_parent()) == nullptr)
		return false;

	if (auto is_created = is_created_; is_created){
		destroy_();
		is_created_ = is_created;
	}

	return true;
}

void winp::menu::item_component::handle_parent_changed_event_(event::tree &e){
	if (is_created_){//Ignore if item wasn't previously created
		is_created_ = false;
		create_();
	}
}

void winp::menu::item_component::handle_index_changed_event_(event::tree &e){
	if (is_created_ && e.get_attached_parent() == get_parent_()){
		destroy_();
		create_();
	}
}

UINT winp::menu::item_component::get_local_id_() const{
	return local_id_;
}

std::size_t winp::menu::item_component::get_absolute_index_() const{
	auto parent = dynamic_cast<menu::tree *>(get_parent_());
	return ((parent == nullptr) ? get_index_() : parent->get_absolute_index_of_(*this, false));
}

winp::ui::surface *winp::menu::item_component::get_popup_() const{
	return nullptr;
}

const std::wstring *winp::menu::item_component::get_label_() const{
	return nullptr;
}

const std::wstring *winp::menu::item_component::get_shortcut_() const{
	return nullptr;
}

HFONT winp::menu::item_component::get_font_() const{
	return font_;
}

bool winp::menu::item_component::set_state_(UINT value){
	auto old_states = states_;
	states_ |= (value & ~get_filtered_states_());
	return ((old_states == states_) ? true : update_states_());
}

bool winp::menu::item_component::remove_state_(UINT value){
	auto old_states = states_;
	states_ &= ~(value & ~get_filtered_states_());
	return ((old_states == states_) ? true : update_states_());
}

UINT winp::menu::item_component::get_states_() const{
	auto group_parent = dynamic_cast<group *>(get_parent_());
	return ((group_parent == nullptr) ? (states_ | get_persistent_states_() | group_parent->get_states_(get_index_())) : (states_ | get_persistent_states_() | group_parent->get_states_(get_index_())));
}

UINT winp::menu::item_component::get_persistent_states_() const{
	return 0u;
}

UINT winp::menu::item_component::get_filtered_states_() const{
	return 0u;
}

bool winp::menu::item_component::has_state_(UINT value) const{
	return ((get_states_() & value) != 0u);
}

bool winp::menu::item_component::has_states_(UINT value) const{
	return ((get_states_() & value) == value);
}

UINT winp::menu::item_component::get_types_() const{
	auto group_parent = dynamic_cast<group *>(get_parent_());
	if (group_parent == nullptr)
		return (is_owner_drawn_() ? (types_ | MFT_OWNERDRAW) : types_);
	return (is_owner_drawn_() ? (types_ | group_parent->get_types_(get_index_()) | MFT_OWNERDRAW) : (types_ | group_parent->get_types_(get_index_())));
}

bool winp::menu::item_component::has_type_(UINT value) const{
	return ((get_types_() & value) == value);
}

bool winp::menu::item_component::is_owner_drawn_() const{
	return (draw_item_event.count_() != 0u || dynamic_cast<const event::draw_item_handler *>(this) != nullptr);
}

bool winp::menu::item_component::is_popup_item_() const{
	auto parent = get_parent_();
	return (parent == nullptr || dynamic_cast<ui::window_surface *>(parent->get_parent_()) == nullptr);
}

HBITMAP winp::menu::item_component::get_bitmap_() const{
	return nullptr;
}

HBITMAP winp::menu::item_component::get_checked_bitmap_() const{
	return nullptr;
}

HBITMAP winp::menu::item_component::get_unchecked_bitmap_() const{
	return nullptr;
}

bool winp::menu::item_component::update_(const MENUITEMINFOW &info){
	if (!is_created_)
		return true;

	auto parent = get_parent_();
	if (parent == nullptr)
		return true;

	auto handle = static_cast<HMENU>(parent->get_handle_());
	if (handle == nullptr)//Parent not created
		return true;

	if (local_id_ == 0u)
		return (SetMenuItemInfoW(handle, static_cast<UINT>(get_absolute_index_()), TRUE, &info) != FALSE);

	return (SetMenuItemInfoW(handle, local_id_, FALSE, &info) != FALSE);
}

bool winp::menu::item_component::update_states_(){
	auto parent = get_parent_();
	if (parent == nullptr)
		return true;

	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_STATE,												//Flags
		0,														//Types
		(get_states_() | dynamic_cast<menu::tree *>(parent)->get_states_(get_index_()))
	});
}

bool winp::menu::item_component::update_types_(){
	auto parent = get_parent_();
	if (parent == nullptr)
		return true;

	return update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_FTYPE,
		get_types_()
	});
}

void winp::menu::item_component::register_id_(){
	if (local_id_ != 0u && id_is_unique_())
		thread_.surface_manager_.id_map_[local_id_] = this;
}

void winp::menu::item_component::generate_id_(std::size_t max_tries){
	if (local_id_ != 0u)
		return;//ID is unique

	for (; max_tries > 0u; --max_tries){
		local_id_ = thread_.menu_random_generator_(static_cast<UINT>(1), std::numeric_limits<UINT>::max());
		if (id_is_unique_())
			break;//ID is unique
	}

	if (max_tries == 0u){//Failed to generate a unique ID
		local_id_ = 0u;
		return;
	}

	thread_.surface_manager_.id_map_[local_id_] = this;
	update_(MENUITEMINFOW{
		sizeof(MENUITEMINFOW),
		MIIM_ID,												//Flags
		0,														//Types
		0,														//States
		local_id_
	});
}

bool winp::menu::item_component::id_is_unique_() const{
	if (thread_.surface_manager_.menu_item_id_is_reserved_(local_id_))
		return false;//Reserved ID

	if (!thread_.surface_manager_.id_map_.empty() && thread_.surface_manager_.id_map_.find(local_id_) != thread_.surface_manager_.id_map_.end())
		return false;//Already in use

	return true;//ID is unique
}
