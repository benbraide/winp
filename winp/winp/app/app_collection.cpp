#include "app_collection.h"

winp::app::object *winp::app::collection::find_main(){
	return main_;
}

void winp::app::collection::each(const std::function<bool(object &)> &callback){
	auto list_copy = list_;
	for (auto item : list_copy){
		if (!callback(*item))
			break;
	}
}

void winp::app::collection::add_(object &target){
	std::lock_guard<std::mutex> guard(lock_);
	list_.push_back(&target);
	if (target.is_main_)
		main_ = &target;
}

void winp::app::collection::remove_(object *target){
	std::lock_guard<std::mutex> guard(lock_);
	if (target == main_)
		main_ = nullptr;

	auto it = std::find(list_.begin(), list_.end(), target);
	if (it != list_.end())
		list_.erase(it);
}

winp::app::object *winp::app::collection::main_ = nullptr;

std::mutex winp::app::collection::lock_;

std::list<winp::app::object *> winp::app::collection::list_;
