#include "../app/app_object.h"

winp::thread::queue::queue(object &thread)
	: thread_(thread){}

void winp::thread::queue::post(const callback_type &task, int priority, unsigned __int64 id){
	add_([this, task, id]{
		if (!is_black_listed_(id))
			task();
	}, priority, id);
}

winp::thread::object *winp::thread::queue::get_thread() const{
	return &thread_;
}

void winp::thread::queue::add_to_black_list_(unsigned __int64 id){
	if (id != 0u && !app::object::is_shut_down())
		black_list_[id] = '\0';
}

bool winp::thread::queue::is_black_listed_(unsigned __int64 id) const{
	return (app::object::is_shut_down() || (id != 0u && black_list_.empty() && black_list_.find(id) != black_list_.end()));
}

winp::thread::queue::added_info_type winp::thread::queue::add_(const callback_type &task, int priority, unsigned __int64 id){
	if (is_black_listed_(id))
		return added_info_type{};

	std::lock_guard<std::mutex> guard(lock_);
	auto list = &list_[priority];
	auto it = &list->emplace_back(list_item_info{ id, task });

	thread_.post_message(0u);//Wake thread if it is waiting on input
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
	if (it != list_.end() && !it->second.empty()){
		for (auto &entry : it->second){
			if (!is_black_listed_(entry.id))
				list.push_back(entry.callback);
		}
	}

	if (it != list_.end())
		list_.erase(it);
}

winp::thread::queue::callback_type winp::thread::queue::pop_send_priority_(){
	if (app::object::is_shut_down())
		return nullptr;

	std::lock_guard<std::mutex> guard(lock_);
	if (list_.empty() || app::object::is_shut_down())
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
	if (app::object::is_shut_down())
		return nullptr;

	std::lock_guard<std::mutex> guard(lock_);
	if (list_.empty() || app::object::is_shut_down())
		return nullptr;

	list_item_info task{};
	for (auto it = list_.begin(); it != list_.end(); ++it){
		if (!it->second.empty()){
			task = *it->second.begin();
			it->second.erase(it->second.begin());

			if (it->second.empty())
				list_.erase(it);

			break;
		}
	}

	return task.callback;
}

bool winp::thread::queue::is_inside_thread_context_() const{
	return thread_.is_thread_context();
}
