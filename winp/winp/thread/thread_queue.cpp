#include "../app/app_object.h"

winp::thread::queue::queue(object &owner)
	: owner_(&owner){
	this->owner.init_(nullptr, nullptr, [this](const prop::base &, void *buf, std::size_t){
		*static_cast<const object **>(buf) = owner_;
	});
}

void winp::thread::queue::post(const callback_type &task, int priority){
	add_(task, priority);
}

winp::thread::queue::added_info_type winp::thread::queue::add_(const callback_type &task, int priority){
	if (app::object::is_shut_down)
		return added_info_type{ nullptr, nullptr };

	bool was_empty;
	std::list<callback_type> *list;
	callback_type *it;

	{//Scoped
		std::lock_guard<std::mutex> guard(lock_);

		was_empty = is_empty_();
		list = &list_[priority];
		it = &list->emplace_back(task);
	}

	if (was_empty)
		cv_.notify_all();

	return added_info_type{ list, it };
}

void winp::thread::queue::pop_all_send_priorities_(std::list<callback_type> &list){
	if (!owner_->inside){
		app::object::error = prop::default_error_mapper::value_type::thread_context_mismatch;
		return;
	}

	std::lock_guard<std::mutex> guard(lock_);

	list.clear();
	if (list_.empty())
		return;

	auto it = list_.find(send_priority);
	if (it == list_.end())
		return;

	if (it->second.empty()){
		list_.erase(it);
		return;
	}

	for (auto &entry : it->second)
		list.push_back(entry);

	list_.erase(it);
}

winp::thread::queue::callback_type winp::thread::queue::pop_send_priority_(){
	if (!owner_->inside){
		app::object::error = prop::default_error_mapper::value_type::thread_context_mismatch;
		return nullptr;
	}

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

	return task;
}

winp::thread::queue::callback_type winp::thread::queue::pop_(){
	callback_type task;
	if (!owner_->inside){
		app::object::error = prop::default_error_mapper::value_type::thread_context_mismatch;
		return task;
	}

	std::lock_guard<std::mutex> guard(lock_);
	if (list_.empty())
		return nullptr;

	for (auto it = list_.begin(); it != list_.end(); ++it){
		if (!it->second.empty()){
			task = *it->second.begin();
			it->second.erase(it->second.begin());
			if (it->second.empty())
				list_.erase(it);
			break;
		}
	}

	return task;
}

void winp::thread::queue::wait_for_tasks_(){
	std::unique_lock<std::mutex> guard(lock_);
	cv_.wait(guard, [this]{
		return !is_empty_();
	});;
}

bool winp::thread::queue::is_empty_() const{
	for (auto &item : list_){
		if (!item.second.empty())
			return false;
	}

	return true;
}

bool winp::thread::queue::is_inside_thread_context_() const{
	return owner_->inside;
}
