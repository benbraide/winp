#include "../app/app_object.h"

winp::ui::grid::column::column(thread::object &thread)
	: child(thread){}

winp::ui::grid::column::column(grid::row &parent)
	: child(parent.get_thread()){
	change_parent_(&parent);
}

winp::ui::grid::column::~column() = default;

int winp::ui::grid::column::compute_fixed_width_(int row_width) const{
	return 0;
}

bool winp::ui::grid::column::is_fixed_() const{
	return false;
}

void winp::ui::grid::column::update_(int width, int height, int x, int y){
	non_window::child::set_dimension_(m_point_type{ x, y }, m_size_type{ width, height });
}

void winp::ui::grid::column::refresh_(){
	auto grid_parent = get_first_ancestor_of_<grid::object, ui::window_surface>();
	if (grid_parent != nullptr)
		grid_parent->update_();
}

winp::ui::grid::fixed_column::fixed_column(thread::object &thread)
	: column(thread){}

winp::ui::grid::fixed_column::fixed_column(grid::row &parent)
	: column(parent.get_thread()){
	change_parent_(&parent);
}

winp::ui::grid::fixed_column::~fixed_column() = default;

int winp::ui::grid::fixed_column::compute_fixed_width_(int row_width) const{
	return get_size_().cx;
}

bool winp::ui::grid::fixed_column::is_fixed_() const{
	return true;
}

winp::ui::grid::proportional_column::proportional_column(thread::object &thread)
	: fixed_column(thread){}

winp::ui::grid::proportional_column::proportional_column(grid::row &parent)
	: fixed_column(parent.get_thread()){
	change_parent_(&parent);
}

winp::ui::grid::proportional_column::~proportional_column() = default;

bool winp::ui::grid::proportional_column::set_proportion(float value, const std::function<void(thread::item &, bool)> &callback){
	return execute_or_post_task([=]{
		return pass_value_to_callback_(callback, set_proportion_(value));
	});
}

float winp::ui::grid::proportional_column::get_proportion(const std::function<void(float)> &callback) const{
	return execute_or_post_([=]{
		return pass_value_to_callback_(callback, get_proportion_());
	}, callback != nullptr);
}

int winp::ui::grid::proportional_column::compute_fixed_width_(int row_width) const{
	return static_cast<int>(row_width * proportion_);
}

bool winp::ui::grid::proportional_column::set_proportion_(float value){
	if (value != proportion_){
		proportion_ = value;
		refresh_();
	}

	return true;
}

float winp::ui::grid::proportional_column::get_proportion_() const{
	return proportion_;
}
