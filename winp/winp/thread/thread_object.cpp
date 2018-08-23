#include "../app/app_object.h"

winp::thread::object::object()
	: queue(&queue_), queue_(*this), is_main_(false){
	init_();
	std::thread([this]{
		id_ = std::this_thread::get_id();
		local_id_ = GetCurrentThreadId();

		run_();

		id_ = std::thread::id();
		local_id_ = 0u;
	}).detach();
}

winp::thread::object::object(bool)
	: queue(&queue_), queue_(*this), is_main_(true){
	init_();
	id_ = std::this_thread::get_id();
	local_id_ = GetCurrentThreadId();
}

winp::thread::object::~object(){
	app::object::threads -= this;
}

void winp::thread::object::each(const std::function<bool(item &)> &callback) const{
	auto list_copy = list_;
	for (auto item : list_copy){
		if (!callback(*item))
			break;
	}
}

void winp::thread::object::add_(item &target){
	std::lock_guard<std::mutex> guard(lock_);
	list_.push_back(&target);
}

void winp::thread::object::remove_(item *target){
	std::lock_guard<std::mutex> guard(lock_);
	auto it = std::find(list_.begin(), list_.end(), target);
	if (it != list_.end())
		list_.erase(it);
}

void winp::thread::object::init_(){
	auto setter = [this](const prop::base<object> &prop, const void *value, std::size_t index){
		if (&prop == &state){
			auto converted = *static_cast<const state_type *>(value);
			switch (converted){
			case state_type::running:
				if (state_ == state_type::nil || state_ == state_type::stopped){
					if (is_main_)
						run_();
				}
				else if (state_ == state_type::suspended)
					state_ = state_type::stopped;
				break;
			default:
				state_ = converted;
			}
		}
	};

	auto getter = [this](const prop::base<object> &prop, void *buf, std::size_t index){
		if (&prop == &id){
			switch (index){
			case 1u:
				*static_cast<std::thread::id *>(buf) = id_;
				break;
			default:
				*static_cast<DWORD *>(buf) = local_id_;
				break;
			}
		}
		if (&prop == &is_main)
			*static_cast<bool *>(buf) = is_main_;
		else if (&prop == &inside)
			*static_cast<bool *>(buf) = (std::this_thread::get_id() == id_);
		else if (&prop == &state)
			*static_cast<state_type *>(buf) = state_;
	};

	is_main.init_(*this, nullptr, nullptr, getter);
	id.init_(*this, nullptr, nullptr, getter);

	state.init_(*this, nullptr, setter, getter);
	inside.init_(*this, nullptr, nullptr, getter);

	app::object::threads += this;
}

int winp::thread::object::run_(){
	state_ = state_type::running;
	while (run_state_())
		run_task_();

	state_ = state_type::stopped;
	return 0;
}

bool winp::thread::object::run_state_() const{
	if (state_ == state_type::suspended){
		
	}

	return (state_ == state_type::running);
}

void winp::thread::object::run_task_(){
	before_task_();

	auto task = get_next_task_();
	if (task == nullptr)
		queue_.wait_for_tasks_();
	else
		task();

	after_task_();
}

winp::thread::object::m_callback_type winp::thread::object::get_next_task_(){
	return queue_.pop_();
}

void winp::thread::object::before_task_(){}

void winp::thread::object::after_task_(){}

LRESULT winp::thread::object::do_send_(const item *receiver, unsigned int msg, WPARAM wparam, LPARAM lparam) const{
	return 0;
}

winp::thread::value_manager::managed_info_type winp::thread::object::pop_managed_(){
	return value_manager_.pop_();
}

std::shared_ptr<winp::thread::value> winp::thread::object::pop_value_(unsigned __int64 key){
	return value_manager_.pop_(key);
}

const winp::thread::object::item_placeholders_type winp::thread::object::item_placeholders = item_placeholders_type{
	std::shared_ptr<item>(new item),
	std::shared_ptr<item>(new item),
	std::shared_ptr<item>(new item),
};
