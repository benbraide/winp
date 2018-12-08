#include "../app/app_object.h"

winp::ui::grid::row::row(thread::object &thread)
	: child(thread){}

winp::ui::grid::row::row(grid::object &parent)
	: child(parent.get_thread()){
	change_parent_(&parent);
}

winp::ui::grid::row::~row() = default;

int winp::ui::grid::row::compute_fixed_height_(int grid_height) const{
	return 0;
}

bool winp::ui::grid::row::is_fixed_() const{
	return false;
}

void winp::ui::grid::row::update_(int width, int height, int x, int y){
	non_window::child::set_dimension_(m_point_type{ x, y }, m_size_type{ width, height });
	if (children_.empty())
		return;

	grid::column *column_child = nullptr;
	std::vector<int> fixed_widths(children_.size());

	auto client_size = get_client_size_();
	auto fixed_width = 0, shared_count = 0, child_index = 0;

	for (auto child : children_){//Compute fixed width
		if ((column_child = dynamic_cast<grid::column *>(child)) != nullptr && column_child->is_fixed_())
			fixed_width += (fixed_widths[child_index] = column_child->compute_fixed_width_(client_size.cx));
		else if (column_child != nullptr)
			++shared_count;
		++child_index;
	}

	child_index = 0;
	auto shared_width = (((client_size.cx - fixed_width) < 0) ? 0 : (client_size.cx - fixed_width));

	for (auto child : children_){//Update columns
		if ((column_child = dynamic_cast<grid::column *>(child)) != nullptr && column_child->is_fixed_()){
			column_child->update_(fixed_widths[child_index], client_size.cy, x, y);
			x += fixed_widths[child_index];
		}
		else if (column_child != nullptr){
			column_child->update_((shared_width / shared_count), client_size.cy, x, y);
			x += (shared_width / shared_count);
		}

		++child_index;
	}
}

void winp::ui::grid::row::refresh_(){
	auto grid_parent = get_first_ancestor_of_<grid::object, ui::window_surface>();
	if (grid_parent != nullptr)
		grid_parent->update_();
}

winp::ui::grid::fixed_row::fixed_row(thread::object &thread)
	: row(thread){}

winp::ui::grid::fixed_row::fixed_row(grid::row &parent)
	: row(parent.get_thread()){
	change_parent_(&parent);
}

winp::ui::grid::fixed_row::~fixed_row() = default;

int winp::ui::grid::fixed_row::compute_fixed_height_(int grid_height) const{
	return get_size_().cx;
}

bool winp::ui::grid::fixed_row::is_fixed_() const{
	return true;
}

winp::ui::grid::proportional_row::proportional_row(thread::object &thread)
	: fixed_row(thread){}

winp::ui::grid::proportional_row::proportional_row(grid::row &parent)
	: fixed_row(parent.get_thread()){
	change_parent_(&parent);
}

winp::ui::grid::proportional_row::~proportional_row() = default;

bool winp::ui::grid::proportional_row::set_proportion(float value, const std::function<void(ui::object &, bool)> &callback){
	if (thread_.is_thread_context()){
		auto result = set_proportion_(value);
		if (callback != nullptr)
			callback(*this, result);
		return result;
	}

	thread_.queue.post([=]{
		auto result = set_proportion_(value);
		if (callback != nullptr)
			callback(*this, result);
	}, thread::queue::send_priority, id_);

	return true;
}

float winp::ui::grid::proportional_row::get_proportion(const std::function<void(float)> &callback) const{
	if (thread_.is_thread_context()){
		auto result = get_proportion_();
		if (callback != nullptr)
			callback(result);
		return result;
	}

	if (callback != nullptr){
		thread_.queue.post([=]{ callback(get_proportion_()); }, thread::queue::send_priority, id_);
		return false;
	}

	return thread_.queue.execute([this]{ return get_proportion_(); }, thread::queue::send_priority, id_);
}

int winp::ui::grid::proportional_row::compute_fixed_height_(int grid_height) const{
	return static_cast<int>(grid_height * proportion_);
}

bool winp::ui::grid::proportional_row::set_proportion_(float value){
	if (value != proportion_){
		proportion_ = value;
		refresh_();
	}

	return true;
}

float winp::ui::grid::proportional_row::get_proportion_() const{
	return proportion_;
}
