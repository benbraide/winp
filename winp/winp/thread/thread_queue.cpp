#include "../app/app_object.h"

winp::thread::queue::queue(object &thread)
	: thread_(thread){}

void winp::thread::queue::post(const callback_type &task, int priority, unsigned __int64 id){
	add_(task, priority, id);
}

winp::thread::object *winp::thread::queue::get_thread() const{
	return &thread_;
}

winp::thread::queue::added_info_type winp::thread::queue::add_(const callback_type &task, int priority, unsigned __int64 id){
	if (app::object::is_shut_down())
		return added_info_type{ nullptr, nullptr };

	bool was_empty;
	std::list<list_item_info> *list;
	list_item_info *it;

	{//Scoped
		std::lock_guard<std::mutex> guard(lock_);

		was_empty = is_empty_();
		list = &list_[priority];
		it = &list->emplace_back(list_item_info{ id, task });
	}

	if (was_empty)
		cv_.notify_all();

	return added_info_type{ list, it };
}

void winp::thread::queue::pop_all_send_priorities_(std::list<callback_type> &list){
	if (app::object::is_shut_down())
		return;

	std::lock_guard<std::mutex> guard(lock_);

	list.clear();
	if (list_.empty())
		return;

	auto it = list_.find(send_priority);
	if (it == list_.end() && !it->second.empty()){
		for (auto &entry : it->second)
			list.push_back(entry.callback);
	}

	if (it != list_.end())
		list_.erase(it);
}

winp::thread::queue::callback_type winp::thread::queue::pop_send_priority_(){
	if (app::object::is_shut_down())
		return nullptr;

	std::lock_guard<std::mutex> guard(lock_);
	if (list_.empty())
		return nullptr;

	auto it = list_.find(send_priority);
	if (it == list_.end())
		return nullptr;

	if (it->second.empty()){
		list_.erase(it);
		return nullptr;
	}

	auto task = *it->second.begin();
	it->second.erase(it->second.begin());

	if (it->second.empty())
		list_.erase(it);

	return task.callback;
}

winp::thread::queue::callback_type winp::thread::queue::pop_(){
	callback_type task;
	if (app::object::is_shut_down())
		return task;

	std::lock_guard<std::mutex> guard(lock_);
	if (list_.empty())
		return nullptr;

	for (auto it = list_.begin(); it != list_.end(); ++it){
		if (!it->second.empty()){
			task = it->second.begin()->callback;
			it->second.erase(it->second.begin());

			if (it->second.empty())
				list_.erase(it);

			break;
		}
	}

	return task;
}

void winp::thread::queue::wait_for_tasks_(){
	if (app::object::is_shut_down())
		return;

	std::unique_lock<std::mutex> guard(lock_);
	cv_.wait(guard, [this]{
		return !is_empty_();
	});;
}

bool winp::thread::queue::is_empty_() const{
	if (app::object::is_shut_down())
		return true;

	for (auto &item : list_){
		if (!item.second.empty())
			return false;
	}

	return true;
}

bool winp::thread::queue::is_inside_thread_context_() const{
	return thread_.is_thread_context();
}
