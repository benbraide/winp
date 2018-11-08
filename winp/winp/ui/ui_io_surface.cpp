#include "../app/app_object.h"

winp::ui::io_surface::io_surface(thread::object &thread)
	: visible_surface(thread){
	mouse_event.leave.thread_ = thread_;
	mouse_event.enter.thread_ = thread_;

	mouse_event.move.thread_ = thread_;
	mouse_event.wheel.thread_ = thread_;

	mouse_event.down.thread_ = thread_;
	mouse_event.up.thread_ = thread_;
	mouse_event.double_click.thread_ = thread_;

	mouse_event.drag.thread_ = thread_;
	mouse_event.drag_end.thread_ = thread_;
}

winp::ui::io_surface::~io_surface() = default;

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
	return (delta.cx >= ::GetSystemMetrics(SM_CXDRAG) || delta.cy >= ::GetSystemMetrics(SM_CYDRAG));
}