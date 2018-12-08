#include "../app/app_object.h"

winp::ui::alignment_modifier::alignment_modifier() = default;

winp::ui::alignment_modifier::alignment_modifier(object &target)
	: target_(&target){}

winp::ui::alignment_modifier::~alignment_modifier() = default;

void winp::ui::alignment_modifier::handle_hook_callback(unsigned int code){
	if ((code & get_hook_code()) != 0u)
		do_modify_alignment_();
}

int winp::ui::alignment_modifier::get_hook_index() const{
	return 9;
}

bool winp::ui::alignment_modifier::set_alignment_modifier_offset(const POINT &value, const std::function<void(alignment_modifier &, bool)> &callback){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr)//Error
		return false;

	if (surface_target->is_thread_context()){
		auto result = set_alignment_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	surface_target->get_thread().queue.post([=]{
		auto result = set_alignment_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, surface_target->get_id());

	return false;
}

bool winp::ui::alignment_modifier::set_alignment_modifier_offset(int x, int y, const std::function<void(alignment_modifier &, bool)> &callback){
	return set_alignment_modifier_offset(POINT{ x, y }, callback);
}

POINT winp::ui::alignment_modifier::get_alignment_modifier_offset(const std::function<void(const POINT &)> &callback) const{
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<const surface *>(this) : dynamic_cast<const surface *>(target_));
	if (surface_target == nullptr)//Error
		return POINT{};

	if (surface_target->is_thread_context()){
		auto &result = get_alignment_modifier_offset_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		surface_target->use_context([=]{ callback(get_alignment_modifier_offset_()); }, thread::queue::send_priority);
		return POINT{};
	}

	return surface_target->execute_using_context([this]{ return get_alignment_modifier_offset_(); }, thread::queue::send_priority);
}

bool winp::ui::alignment_modifier::set_alignment_modifier_alignment(unsigned int value, const std::function<void(alignment_modifier &, bool)> &callback){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr)//Error
		return false;

	if (surface_target->is_thread_context()){
		auto result = set_alignment_modifier_alignment_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	surface_target->get_thread().queue.post([=]{
		auto result = set_alignment_modifier_alignment_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, surface_target->get_id());

	return false;
}

unsigned int winp::ui::alignment_modifier::get_alignment_modifier_alignment(const std::function<void(unsigned int)> &callback) const{
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<const surface *>(this) : dynamic_cast<const surface *>(target_));
	if (surface_target == nullptr)//Error
		return align_nil;

	if (surface_target->is_thread_context()){
		auto result = get_alignment_modifier_alignment_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		surface_target->use_context([=]{ callback(get_alignment_modifier_alignment_()); }, thread::queue::send_priority);
		return align_nil;
	}

	return surface_target->execute_using_context([this]{ return get_alignment_modifier_alignment_(); }, thread::queue::send_priority);
}

bool winp::ui::alignment_modifier::set_alignment_modifier_offset_(const POINT &value){
	if (value.x != offset_.x || value.y != offset_.y){
		offset_ = value;
		modify_alignment_();
	}

	return true;
}

const POINT &winp::ui::alignment_modifier::get_alignment_modifier_offset_() const{
	return offset_;
}

bool winp::ui::alignment_modifier::set_alignment_modifier_alignment_(unsigned int value){
	if (value != alignment_){
		alignment_ = value;
		modify_alignment_();
	}

	return true;
}

unsigned int winp::ui::alignment_modifier::get_alignment_modifier_alignment_() const{
	return alignment_;
}

void winp::ui::alignment_modifier::modify_alignment_(){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr)//Ignore
		return;

	auto surface_parent = surface_target->get_first_ancestor_of<surface>();
	if (surface_parent != nullptr)
		call_hook_in_target_(*surface_parent);
}

void winp::ui::alignment_modifier::do_modify_alignment_(){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr || !surface_target->is_thread_context())
		return;//Ignore

	auto surface_parent = surface_target->get_first_ancestor_of<surface>();
	if (surface_parent == nullptr)
		return;//Ignore

	POINT position{};
	auto size = surface_target->get_size(), parent_size = surface_parent->get_client_size();

	if ((alignment_ & align_left) != 0u)
		position.x = 0;
	else if ((alignment_ & align_center) != 0u)
		position.x = ((parent_size.cx - size.cx) / 2);
	else if ((alignment_ & align_right) != 0u)
		position.x = (parent_size.cx - size.cx);

	if ((alignment_ & align_left) != 0u)
		position.y = 0;
	else if ((alignment_ & align_center) != 0u)
		position.y = ((parent_size.cy - size.cy) / 2);
	else if ((alignment_ & align_right) != 0u)
		position.y = (parent_size.cy - size.cy);

	surface_target->set_position(position);
}

winp::ui::size_modifier::size_modifier() = default;

winp::ui::size_modifier::size_modifier(object &target)
	: target_(&target){}

winp::ui::size_modifier::~size_modifier() = default;

void winp::ui::size_modifier::handle_hook_callback(unsigned int code){
	if ((code & get_hook_code()) != 0u)
		do_modify_size_();
}

int winp::ui::size_modifier::get_hook_index() const{
	return 0;
}

bool winp::ui::size_modifier::set_size_modifier_offset(const SIZE &value, const std::function<void(size_modifier &, bool)> &callback){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr)//Error
		return false;

	if (surface_target->is_thread_context()){
		auto result = set_size_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	surface_target->get_thread().queue.post([=]{
		auto result = set_size_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, surface_target->get_id());

	return false;
}

bool winp::ui::size_modifier::set_size_modifier_offset(int width, int height, const std::function<void(size_modifier &, bool)> &callback){
	return set_size_modifier_offset(SIZE{ width, height }, callback);
}

bool winp::ui::size_modifier::set_size_modifier_offset(const relative_size &value, const std::function<void(size_modifier &, bool)> &callback){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr)//Error
		return false;

	if (surface_target->is_thread_context()){
		auto result = set_size_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	surface_target->get_thread().queue.post([=]{
		auto result = set_size_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, surface_target->get_id());

	return false;
}

bool winp::ui::size_modifier::set_size_modifier_offset(float width, float height, const std::function<void(size_modifier &, bool)> &callback){
	return set_size_modifier_offset(relative_size{ width, height }, callback);
}

void winp::ui::size_modifier::modify_size_(){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target != nullptr)
		call_hook_in_target_(*surface_target);
}

void winp::ui::size_modifier::do_modify_size_(){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr || !surface_target->is_thread_context())
		return;//Ignore

	auto surface_parent = surface_target->get_first_ancestor_of<surface>();
	if (surface_parent == nullptr)
		return;//Ignore

	auto parent_size = surface_parent->get_client_size(), offset = SIZE{};
	if (std::holds_alternative<relative_size>(offset_)){//Proportional offset
		auto psize = std::get<relative_size>(offset_);
		{//Compute offset
			offset.cx = static_cast<int>(parent_size.cx * (1.0f - psize.width));
			offset.cy = static_cast<int>(parent_size.cy * (1.0f - psize.height));
		}
	}
	else//Fixed size
		offset = std::get<SIZE>(offset_);

	surface_target->set_size(SIZE{ (parent_size.cx - offset.cx), (parent_size.cy - offset.cy) });
}
