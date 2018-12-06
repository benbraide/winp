#include "../app/app_object.h"

winp::ui::alignment_modifier::alignment_modifier() = default;

winp::ui::alignment_modifier::alignment_modifier(object &target)
	: target_(&target){}

winp::ui::alignment_modifier::~alignment_modifier() = default;

void winp::ui::alignment_modifier::handle_hook_callback(unsigned int code){
	if ((code & get_hook_code()) != 0u)
		align_();
}

bool winp::ui::alignment_modifier::set_modifier_offset(const POINT &value, const std::function<void(alignment_modifier &, bool)> &callback){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr)//Error
		return false;

	if (surface_target->is_thread_context()){
		auto result = set_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	surface_target->get_thread().queue.post([=]{
		auto result = set_modifier_offset_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, surface_target->get_id());

	return false;
}

POINT winp::ui::alignment_modifier::get_modifier_offset(const std::function<void(const POINT &)> &callback) const{
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<const surface *>(this) : dynamic_cast<const surface *>(target_));
	if (surface_target == nullptr)//Error
		return POINT{};

	if (surface_target->is_thread_context()){
		auto &result = get_modifier_offset_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		surface_target->use_context([=]{ callback(get_modifier_offset_()); }, thread::queue::send_priority);
		return POINT{};
	}

	return surface_target->execute_using_context([this]{ return get_modifier_offset_(); }, thread::queue::send_priority);
}

bool winp::ui::alignment_modifier::set_modifier_alignment(unsigned int value, const std::function<void(alignment_modifier &, bool)> &callback){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr)//Error
		return false;

	if (surface_target->is_thread_context()){
		auto result = set_modifier_alignment_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	surface_target->get_thread().queue.post([=]{
		auto result = set_modifier_alignment_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, surface_target->get_id());

	return false;
}

unsigned int winp::ui::alignment_modifier::get_modifier_alignment(const std::function<void(unsigned int)> &callback) const{
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<const surface *>(this) : dynamic_cast<const surface *>(target_));
	if (surface_target == nullptr)//Error
		return align_nil;

	if (surface_target->is_thread_context()){
		auto result = get_modifier_alignment_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		surface_target->use_context([=]{ callback(get_modifier_alignment_()); }, thread::queue::send_priority);
		return align_nil;
	}

	return surface_target->execute_using_context([this]{ return get_modifier_alignment_(); }, thread::queue::send_priority);
}

bool winp::ui::alignment_modifier::set_modifier_offset_(const POINT &value){
	if (value.x != offset_.x || value.y != offset_.y){
		offset_ = value;
		align_();
	}

	return true;
}

const POINT &winp::ui::alignment_modifier::get_modifier_offset_() const{
	return offset_;
}

bool winp::ui::alignment_modifier::set_modifier_alignment_(unsigned int value){
	if (value != alignment_){
		alignment_ = value;
		align_();
	}

	return true;
}

unsigned int winp::ui::alignment_modifier::get_modifier_alignment_() const{
	return alignment_;
}

void winp::ui::alignment_modifier::align_(){
	auto surface_target = ((target_ == nullptr) ? dynamic_cast<surface *>(this) : dynamic_cast<surface *>(target_));
	if (surface_target == nullptr || !surface_target->is_thread_context())
		return;//Ignore

	auto surface_parent = surface_target->get_first_ancestor_of<surface>();
	if (surface_parent == nullptr)
		return;//Ignore

	POINT position{};
	auto size = surface_target->get_size(), parent_size = surface_parent->get_size();

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
