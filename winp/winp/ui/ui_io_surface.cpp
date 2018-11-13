#include "../app/app_object.h"

winp::ui::io_surface::mouse_event_info::mouse_event_info(io_surface &owner)
	: leave(owner), enter(owner), move(owner), wheel(owner), down(owner), up(owner), double_click(owner), drag(owner), drag_begin(owner), drag_end(owner){}

winp::ui::io_surface::key_event_info::key_event_info(io_surface &owner)
	: down(owner), up(owner), press(owner){}

winp::ui::io_surface::io_surface() = default;

winp::ui::io_surface::io_surface(thread::object &thread)
	: visible_surface(thread){}

winp::ui::io_surface::~io_surface() = default;

winp::ui::io_surface *winp::ui::io_surface::get_io_surface_parent_() const{
	return dynamic_cast<io_surface *>(get_parent_());
}

winp::ui::io_surface *winp::ui::io_surface::get_top_moused_() const{
	return ((moused_ == nullptr || dynamic_cast<window_surface *>(moused_) != nullptr) ? const_cast<io_surface *>(this) : moused_->get_top_moused_());
}

winp::ui::io_surface *winp::ui::io_surface::find_moused_child_(const m_point_type &position) const{
	io_surface *io_child = nullptr;
	for (auto child : children_){
		if ((io_child = dynamic_cast<io_surface *>(io_child)) != nullptr && dynamic_cast<window_surface *>(io_child) == nullptr && io_child->hit_test_(position, true) == utility::hit_target::inside)
			return io_child;
	}

	return nullptr;
}

winp::ui::io_surface *winp::ui::io_surface::get_drag_target_(const m_size_type &delta) const{
	return ((GetSystemMetrics(SM_CXDRAG) <= delta.cx || GetSystemMetrics(SM_CYDRAG) <= delta.cy) ? const_cast<io_surface *>(this) : nullptr);
}

bool winp::ui::io_surface::is_dialog_message_(MSG &msg) const{
	auto parent = get_first_ancestor_of_<io_surface>();
	return (parent != nullptr && parent->is_dialog_message_(msg));
}
