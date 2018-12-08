#include "../app/app_object.h"

winp::ui::grid::object::object(thread::object &thread)
	: child(thread){}

winp::ui::grid::object::object(ui::tree &parent)
	: child(parent.get_thread()){
	change_parent_(&parent);
}

winp::ui::grid::object::~object() = default;

bool winp::ui::grid::object::set_size_(const m_size_type &value){
	if (non_window::child::set_size_(value)){
		update_();
		return true;
	}

	return false;
}

void winp::ui::grid::object::update_(){
	if (children_.empty())
		return;

	grid::row *row_child = nullptr;
	std::vector<int> fixed_heights(children_.size());

	auto client_size = get_client_size_();
	auto fixed_height = 0, shared_count = 0, child_index = 0, y = 0;

	for (auto child : children_){//Compute fixed height
		if ((row_child = dynamic_cast<grid::row *>(child)) != nullptr && row_child->is_fixed_())
			fixed_height += (fixed_heights[child_index] = row_child->compute_fixed_height_(client_size.cy));
		else if (row_child != nullptr)
			++shared_count;
		++child_index;
	}

	child_index = 0;
	auto shared_height = (((client_size.cy - fixed_height) < 0) ? 0 : (client_size.cy - fixed_height));

	for (auto child : children_){//Update rows
		if ((row_child = dynamic_cast<grid::row *>(child)) != nullptr && row_child->is_fixed_()){
			row_child->update_(client_size.cx, fixed_heights[child_index], 0, y);
			y += fixed_heights[child_index];
		}
		else if (row_child != nullptr){
			row_child->update_(client_size.cx, (shared_height / shared_count), 0, y);
			y += (shared_height / shared_count);
		}

		++child_index;
	}
}
