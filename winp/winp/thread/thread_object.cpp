#include "../app/app_object.h"

winp::thread::object::object()
	: queue_(*this), is_main_(false), is_exiting_(false){
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
	: queue_(*this), is_main_(true), is_exiting_(false){
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
	std::lock_guard<std::mutex> guard(queue_.lock_);
	list_.push_back(&target);
}

void winp::thread::object::remove_(item *target){
	std::lock_guard<std::mutex> guard(queue_.lock_);
	auto it = std::find(list_.begin(), list_.end(), target);
	if (it != list_.end())
		list_.erase(it);
}

void winp::thread::object::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t index){
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
		else if (&prop == &is_main)
			*static_cast<bool *>(buf) = is_main_;
		else if (&prop == &inside)
			*static_cast<bool *>(buf) = (std::this_thread::get_id() == id_);
	};

	queue.m_value_ = &queue_;
	is_main.init_(nullptr, nullptr, getter);
	id.init_(nullptr, nullptr, getter);
	inside.init_(nullptr, nullptr, getter);

	app::object::threads += this;
}

int winp::thread::object::run_(){
	MSG msg;
	auto value = 0;

	while (true){
		if (GetMessageW(&msg, nullptr, 0u, 0u) == -1 || app::object::is_shut_down_){
			value = -1;
			break;
		}

		if (msg.message == WM_QUIT){//Quit message posted
			value = static_cast<int>(msg.wParam);
			break;
		}
	}

	is_exiting_ = true;
	while (run_task_()){}//Execute remaining tasks

	std::lock_guard<std::mutex> guard(queue_.lock_);
	for (auto item : list_)
		item->destroy_();

	return value;
}

bool winp::thread::object::run_task_(){
	auto task = get_next_task_();
	if (task == nullptr)
		return false;

	before_task_(task);
	task();
	after_task_(task);

	return true;
}

winp::thread::object::m_callback_type winp::thread::object::get_next_task_(){
	return queue_.pop_();
}

void winp::thread::object::before_task_(m_callback_type &task){}

void winp::thread::object::after_task_(m_callback_type &task){}
