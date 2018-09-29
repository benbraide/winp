#include "ui_visible_surface.h"

winp::ui::visible_surface::visible_surface(thread::object &thread)
	: surface(thread){
	init_();
}

winp::ui::visible_surface::visible_surface(tree &parent)
	: surface(parent){
	init_();
}

winp::ui::visible_surface::~visible_surface() = default;

void winp::ui::visible_surface::init_(){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
		if (&prop == &visible){
			auto tval = *static_cast<const bool *>(value);
			owner_->queue->post([=]{
				set_visible_state_(tval);
			}, thread::queue::send_priority);
		}
		else if (&prop == &transparent){
			auto tval = *static_cast<const bool *>(value);
			owner_->queue->post([=]{
				set_transaprent_state_(tval);
			}, thread::queue::send_priority);
		}
		else if (&prop == &background_color){
			auto tval = *static_cast<const m_rgba_type *>(value);
			owner_->queue->post([=]{
				set_background_color_(tval);
			}, thread::queue::send_priority);
		}
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &visible)
			*static_cast<bool *>(buf) = owner_->queue->add([this]{ return get_visible_state_(); }, thread::queue::send_priority).get();
		else if (&prop == &transparent)
			*static_cast<bool *>(buf) = owner_->queue->add([this]{ return get_transaprent_state_(); }, thread::queue::send_priority).get();
		else if (&prop == &background_color)
			*static_cast<m_rgba_type *>(buf) = owner_->queue->add([this]{ return get_background_color_(); }, thread::queue::send_priority).get();
	};

	visible.init_(nullptr, setter, getter);
	transparent.init_(nullptr, setter, getter);
	background_color.init_(nullptr, setter, getter);

	show_event_.thread_ = owner_;
	hide_event_.thread_ = owner_;
}

void winp::ui::visible_surface::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(visibility)){
		if (get_visible_state_())
			*static_cast<visibility *>(buf) = (get_transaprent_state_() ? visibility::transparent : visibility::visible);
		else
			*static_cast<visibility *>(buf) = visibility::hidden;
	}
	else if (id == typeid(show_event_type))
		*static_cast<show_event_type *>(buf) = show_event_type(show_event_);
	else if (id == typeid(hide_event_type))
		*static_cast<hide_event_type *>(buf) = hide_event_type(hide_event_);
	else if (id == typeid(visible_surface *))
		*static_cast<visible_surface **>(buf) = this;
	else
		surface::do_request_(buf, id);
}

void winp::ui::visible_surface::do_apply_(const void *value, const std::type_info &id){
	if (id == typeid(visibility)){
		switch (*static_cast<const visibility *>(value)){
		case visibility::visible:
			set_visible_state_(true);
			break;
		case visibility::hidden:
			set_visible_state_(false);
			break;
		case visibility::transparent:
			set_transaprent_state_(true);
			break;
		default:
			break;
		}
	}
	else
		surface::do_apply_(value, id);
}

winp::ui::visible_surface *winp::ui::visible_surface::get_visible_surface_parent_() const{
	return dynamic_cast<visible_surface *>(get_parent_());
}

/*
void winp::ui::visible_surface::toggle_state_(unsigned int value, bool set){
	if (set)
		set_state_(value);
	else
		remove_state_(value);
}

void winp::ui::visible_surface::set_state_(unsigned int value){
	state_ |= value;
}

void winp::ui::visible_surface::remove_state_(unsigned int value){
	state_ &= ~value;
}

bool winp::ui::visible_surface::has_state_(unsigned int value) const{
	return ((state_ & value) == value);
}*/

void winp::ui::visible_surface::set_visible_state_(bool state){
	//toggle_state_(state_visible, state);
}

bool winp::ui::visible_surface::get_visible_state_() const{
	//return has_state_(state_visible);
	return false;
}

void winp::ui::visible_surface::set_transaprent_state_(bool state){
	//toggle_state_(state_transparent, state);
}

bool winp::ui::visible_surface::get_transaprent_state_() const{
	//return has_state_(state_transparent);
	return false;
}

void winp::ui::visible_surface::set_background_color_(const m_rgba_type &value){
	background_color_ = value;
}

winp::ui::visible_surface::m_rgba_type winp::ui::visible_surface::get_background_color_() const{
	return background_color_;
}
