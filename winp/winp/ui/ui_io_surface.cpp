#include "../app/app_object.h"

winp::ui::io_surface::io_surface(thread::object &thread)
	: visible_surface(thread){}

winp::ui::io_surface::io_surface(tree &parent)
	: visible_surface(parent){}

winp::ui::io_surface::~io_surface() = default;

void winp::ui::io_surface::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(io_surface *))
		*static_cast<io_surface **>(buf) = this;
	else
		visible_surface::do_request_(buf, id);
}

winp::ui::io_surface *winp::ui::io_surface::get_io_surface_parent_() const{
	return dynamic_cast<io_surface *>(get_parent_());
}

winp::ui::io_surface *winp::ui::io_surface::get_top_moused_() const{
	return ((moused_ == nullptr) ? const_cast<io_surface *>(this) : moused_->get_top_moused_());
}

winp::ui::io_surface *winp::ui::io_surface::find_moused_child_(const m_point_type &position) const{
	io_surface *io_child = nullptr;
	for (auto child : children_){
		if ((io_child = dynamic_cast<io_surface *>(io_child)) != nullptr && dynamic_cast<window_surface *>(io_child) == nullptr && io_child->hit_test_(position, true) == utility::hit_target::inside)
			return io_child;
	}

	return nullptr;
}

bool winp::ui::io_surface::should_begin_drag_(const m_size_type &delta) const{
	return (delta.width >= ::GetSystemMetrics(SM_CXDRAG) || delta.height >= ::GetSystemMetrics(SM_CYDRAG));
}
