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

bool winp::ui::io_surface::is_dialog_message_(MSG &msg) const{
	auto parent = get_first_ancestor_of_<io_surface>();
	return (parent != nullptr && parent->is_dialog_message_(msg));
}
