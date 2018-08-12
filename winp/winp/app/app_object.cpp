#include "app_collection.h"

winp::app::object::object(){

}

winp::app::object::~object() = default;

void winp::app::object::each(const std::function<bool(m_thread_type &)> &callback) const{
	auto list_copy = list_;
	for (auto item : list_copy){
		if (!callback(*item))
			break;
	}
}

winp::app::object::m_thread_type *winp::app::object::find(std::thread::id id) const{
	std::lock_guard<std::mutex> guard(lock_);
	for (auto item : list_){
		if (item->id_ == id)
			return item;
	}

	return nullptr;
}

winp::app::object::m_thread_type *winp::app::object::find(DWORD id) const{
	std::lock_guard<std::mutex> guard(lock_);
	for (auto item : list_){
		if (item->local_id_ == id)
			return item;
	}

	return nullptr;
}

winp::app::object::m_thread_type *winp::app::object::find_main(){
	return main_;
}

void winp::app::object::add_(m_thread_type &target){
	std::lock_guard<std::mutex> guard(lock_);
	list_.push_back(&target);
	if (target.is_main_)
		main_ = &target;
}

void winp::app::object::remove_(m_thread_type *target){
	std::lock_guard<std::mutex> guard(lock_);
	if (target == main_)
		main_ = nullptr;

	auto it = std::find(list_.begin(), list_.end(), target);
	if (it != list_.end())
		list_.erase(it);
}


winp::thread::object *winp::app::object::main_ = nullptr;
