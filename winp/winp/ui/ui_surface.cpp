#include "ui_surface.h"

winp::ui::surface_hit_test::surface_hit_test() = default;

winp::ui::surface_hit_test::surface_hit_test(const surface_hit_test &copy)
	: surface_(copy.surface_){
	if (surface_ != nullptr)
		init_();
}

winp::ui::surface_hit_test &winp::ui::surface_hit_test::operator=(const surface_hit_test &copy){
	if ((surface_ = copy.surface_) != nullptr && target.getter_ == nullptr)
		init_();
	return *this;
}

void winp::ui::surface_hit_test::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &target){
			auto pos = (is_absolute.m_value_ ? surface_->get_absolute_position_() : surface_->get_position_());
			auto client_offset = surface_->get_client_position_offset_();
			{//Update position
				pos.x += client_offset.width;
				pos.y += client_offset.height;
			}

			auto size = surface_->get_size_();
			if (std::holds_alternative<m_rect_type>(value.m_value_)){
				auto &rect = std::get<m_rect_type>(value.m_value_);
				if (hit_test_(m_point_type{ rect.left, rect.top }, pos, size) == utility::hit_target::inside && hit_test_(m_point_type{ rect.right, rect.bottom }, pos, size) == utility::hit_target::inside)
					*static_cast<utility::hit_target *>(buf) = utility::hit_target::inside;
				else if (pos.x < rect.right && pos.y < rect.bottom && (pos.x + size.width) > rect.left && (pos.y + size.height) > rect.top)
					*static_cast<utility::hit_target *>(buf) = utility::hit_target::intersect;
				else
					*static_cast<utility::hit_target *>(buf) = utility::hit_target::nil;
			}
			else
				*static_cast<utility::hit_target *>(buf) = hit_test_(std::get<m_point_type>(value.m_value_), pos, size);
		}
	};

	target.init_(nullptr, nullptr, getter);
}

winp::utility::hit_target winp::ui::surface_hit_test::hit_test_(const m_point_type &pt, const m_point_type &pos, const m_size_type &size) const{
	if (pt.x < pos.x || pt.y < pos.y)
		return utility::hit_target::nil;

	if (pt.x < (pos.x + size.width) || pt.y < (pos.y + size.height))
		return utility::hit_target::inside;

	return utility::hit_target::nil;
}

winp::ui::surface::surface(thread::object &thread)
	: tree(thread){
	init_();
}

winp::ui::surface::surface(tree &parent)
	: tree(parent){
	init_();
}

winp::ui::surface::~surface() = default;

void winp::ui::surface::init_(){
	auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
		if (&prop == &size){
			auto tval = *static_cast<const m_size_type *>(value);
			owner_->queue->post([=]{
				set_size_(tval);
			}, thread::queue::send_priority);
		}
		else if (&prop == &position){
			auto tval = *static_cast<const m_point_type *>(value);
			owner_->queue->post([=]{
				set_position_(tval);
			}, thread::queue::send_priority);
		}
		else if (&prop == &absolute_position){
			auto tval = *static_cast<const m_point_type *>(value);
			owner_->queue->post([=]{
				set_absolute_position_(tval);
			}, thread::queue::send_priority);
		}
	};

	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &size)
			*static_cast<m_size_type *>(buf) = owner_->queue->add([this]{ return get_size_(); }, thread::queue::send_priority).get();
		else if (&prop == &client_position_offset)
			*static_cast<m_size_type *>(buf) = owner_->queue->add([this]{ return get_client_position_offset_(); }, thread::queue::send_priority).get();
		else if (&prop == &position)
			*static_cast<m_point_type *>(buf) = owner_->queue->add([this]{ return get_position_(); }, thread::queue::send_priority).get();
		else if (&prop == &absolute_position)
			*static_cast<m_point_type *>(buf) = owner_->queue->add([this]{ return get_absolute_position_(); }, thread::queue::send_priority).get();
		else if (&prop == &dimension)
			*static_cast<m_rect_type *>(buf) = owner_->queue->add([this]{ return get_dimension_(); }, thread::queue::send_priority).get();
		else if (&prop == &absolute_dimension)
			*static_cast<m_rect_type *>(buf) = owner_->queue->add([this]{ return get_absolute_dimension_(); }, thread::queue::send_priority).get();
	};

	size.init_(nullptr, setter, getter);
	client_position_offset.init_(nullptr, nullptr, getter);

	position.init_(nullptr, setter, getter);
	absolute_position.init_(nullptr, setter, getter);

	dimension.init_(nullptr, nullptr, getter);
	absolute_dimension.init_(nullptr, nullptr, getter);

	size_event_.thread_ = owner_;
	move_event_.thread_ = owner_;
}

void winp::ui::surface::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(surface_hit_test)){
		static_cast<surface_hit_test *>(buf)->surface_ = this;
		static_cast<surface_hit_test *>(buf)->init_();
	}
	else if (id == typeid(size_event_type))
		*static_cast<size_event_type *>(buf) = size_event_type(size_event_);
	else if (id == typeid(move_event_type))
		*static_cast<move_event_type *>(buf) = move_event_type(move_event_);
	else if (id == typeid(surface *))
		*static_cast<surface **>(buf) = this;
	else
		tree::do_request_(buf, id);
}

winp::ui::surface *winp::ui::surface::get_surface_parent_() const{
	return dynamic_cast<surface *>(get_parent_());
}

void winp::ui::surface::set_size_(const m_size_type &value){
	size_ = value;
}

winp::ui::surface::m_size_type winp::ui::surface::get_size_() const{
	return size_;
}

winp::ui::surface::m_size_type winp::ui::surface::get_client_position_offset_() const{
	return m_size_type{};
}

void winp::ui::surface::set_position_(const m_point_type &value){
	position_ = value;
}

winp::ui::surface::m_point_type winp::ui::surface::get_position_() const{
	return position_;
}

void winp::ui::surface::set_absolute_position_(const m_point_type &value){
	auto surface_parent = get_surface_parent_();
	set_position_((surface_parent == nullptr) ? value : surface_parent->convert_position_from_absolute_value_(value));
}

winp::ui::surface::m_point_type winp::ui::surface::get_absolute_position_() const{
	auto surface_parent = get_surface_parent_();
	return ((surface_parent == nullptr) ? get_position_() : surface_parent->convert_position_to_absolute_value_(get_position_()));
}

winp::ui::surface::m_rect_type winp::ui::surface::get_dimension_() const{
	auto size = get_size_();
	auto position = get_position_();
	
	return m_rect_type{
		position.x,
		position.y,
		(position.x + size.width),
		(position.y + size.height)
	};
}

winp::ui::surface::m_rect_type winp::ui::surface::get_absolute_dimension_() const{
	auto size = get_size_();
	auto position = get_absolute_position_();

	return m_rect_type{
		position.x,
		position.y,
		(position.x + size.width),
		(position.y + size.height)
	};
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_from_absolute_value_(const m_point_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	return m_point_type{ (value.x - absolute_position.x - client_offset.width), (value.y - absolute_position.y - client_offset.height) };
}

winp::ui::surface::m_point_type winp::ui::surface::convert_position_to_absolute_value_(const m_point_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	return m_point_type{ (value.x + absolute_position.x + client_offset.width), (value.y + absolute_position.y + client_offset.height) };
}

winp::ui::surface::m_rect_type winp::ui::surface::convert_dimension_from_absolute_value_(const m_rect_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	auto h_offset = (absolute_position.x - client_offset.width);
	auto v_offset = (absolute_position.y - client_offset.height);

	return m_rect_type{ (value.left - h_offset), (value.top - v_offset), (value.right - h_offset), (value.bottom - v_offset) };
}

winp::ui::surface::m_rect_type winp::ui::surface::convert_dimension_to_absolute_value_(const m_rect_type &value) const{
	auto absolute_position = get_absolute_position_();
	auto client_offset = get_client_position_offset_();

	auto h_offset = (absolute_position.x + client_offset.width);
	auto v_offset = (absolute_position.y + client_offset.height);

	return m_rect_type{ (value.left + h_offset), (value.top + v_offset), (value.right + h_offset), (value.bottom + v_offset) };
}
