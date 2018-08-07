#include "thread_object.h"

winp::thread::queue::queue(object &owner)
	: owner_(&owner){
	this->owner.init_(*this, nullptr, nullptr, [this](const prop::base<queue> &, void *buf, std::size_t){
		*static_cast<object **>(buf) = owner_;
	}, &owner_->error);
}

winp::thread::queue::added_info_type winp::thread::queue::add_(const callback_type &task, int priority){
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

winp::thread::queue::callback_type winp::thread::queue::pop_(){
	callback_type task;
	if (!owner_->inside){
		owner_->error = object::thread_context_mismatch;
		return task;
	}

	std::lock_guard<std::mutex> guard(lock_);
	for (auto top = list_.rbegin(); top  != list_.rend(); ++top){
		if (!top->second.empty()){
			task = *top->second.begin();
			top->second.erase(top->second.begin());
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
