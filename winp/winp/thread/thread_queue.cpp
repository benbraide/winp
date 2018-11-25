#include "../app/app_object.h"

winp::thread::queue::queue(object &thread)
	: thread_(thread){}

void winp::thread::queue::post(const callback_type &task, int priority, unsigned __int64 id){
	add_([=]{
		if (!is_black_listed_(id))
			task();
	}, priority);
}

winp::thread::object &winp::thread::queue::get_thread(){
	return thread_;
}

void winp::thread::queue::add_to_black_list_(unsigned __int64 id){
	if (id != 0u && !app::object::is_shut_down())
		black_list_[id] = '\0';
}

void winp::thread::queue::remove_from_black_list_(unsigned __int64 id){
	if (id != 0u && !app::object::is_shut_down())
		black_list_.erase(id);
}

bool winp::thread::queue::is_black_listed_(unsigned __int64 id) const{
	return (app::object::is_shut_down() || (id != 0u && black_list_.empty() && black_list_.find(id) != black_list_.end()));
}

void winp::thread::queue::add_(const callback_type &task, int priority){
	std::lock_guard<std::mutex> guard(lock_);
	map_[priority].push_back(task);
}

void winp::thread::queue::pop_all_send_priorities_(std::list<callback_type> &list){
	std::lock_guard<std::mutex> guard(lock_);

	list.clear();
	if (map_.empty())
		return;

	auto it = map_.find(send_priority);
	if (it != map_.end() && !it->second.empty()){
		list = std::move(it->second);
		map_.erase(it);
	}
}

winp::thread::queue::callback_type winp::thread::queue::pop_send_priority_(){
	std::lock_guard<std::mutex> guard(lock_);
	if (map_.empty())
		return nullptr;

	auto it = map_.find(send_priority);
	if (it == map_.end())
		return nullptr;

	if (it->second.empty()){
		map_.erase(it);
		return nullptr;
	}

	auto task = *it->second.begin();
	it->second.erase(it->second.begin());

	if (it->second.empty())
		map_.erase(it);

	return task;
}

winp::thread::queue::callback_type winp::thread::queue::pop_(){
	if (app::object::is_shut_down())
		return nullptr;

	std::lock_guard<std::mutex> guard(lock_);
	if (map_.empty() || app::object::is_shut_down())
		return nullptr;

	callback_type task;
	for (auto it = map_.begin(); it != map_.end(); it = map_.begin()){
		if (!it->second.empty()){
			task = *it->second.begin();
			it->second.erase(it->second.begin());

			if (it->second.empty())
				map_.erase(it);

			break;
		}

		map_.erase(it);
	}

	return task;
}

bool winp::thread::queue::is_inside_thread_context_() const{
	return thread_.is_thread_context();
}
