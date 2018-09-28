#include "../app/app_object.h"

winp::thread::post_message::post_message() = default;

winp::thread::post_message::post_message(const post_message &copy)
	: target_(copy.target_){
	if (target_ != nullptr)
		init_();
}

winp::thread::post_message &winp::thread::post_message::operator=(const post_message &copy){
	if ((target_ = copy.target_) != nullptr && target.getter_ == nullptr)
		init_();
	return *this;
}

void winp::thread::post_message::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &target)
			*static_cast<object **>(buf) = target_;
		else if (&prop == &result)
			*static_cast<bool *>(buf) = (PostThreadMessageW(target_->id, code.m_value_, wparam.m_value_, lparam.m_value_) != FALSE);
	};

	target.init_(nullptr, nullptr, getter);
	result.init_(nullptr, nullptr, getter);
}

winp::thread::object::object()
	: queue_(*this), is_main_(false), is_exiting_(false){
	init_();
	std::thread([this]{
		id_ = std::this_thread::get_id();
		local_id_ = GetCurrentThreadId();
		app::object::current_thread_ = this;

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
	app::object::current_thread_ = this;
}

winp::thread::object::~object(){
	app::object::threads -= this;
}

void winp::thread::object::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &id){
			switch (context){
			case 0u:
				*static_cast<DWORD *>(buf) = local_id_;
				break;
			case 1u:
				*static_cast<std::thread::id *>(buf) = id_;
				break;
			default:
				break;
			}
		}
		else if (&prop == &is_main)
			*static_cast<bool *>(buf) = is_main_;
		else if (&prop == &inside)
			*static_cast<bool *>(buf) = (std::this_thread::get_id() == id_);
		else if (&prop == &request)
			do_request_(buf, *reinterpret_cast<std::type_info *>(context));
	};

	queue.m_value_ = &queue_;
	is_main.init_(nullptr, nullptr, getter);

	id.init_(nullptr, nullptr, getter);
	inside.init_(nullptr, nullptr, getter);
	request.init_(nullptr, nullptr, getter);

	app::object::threads += this;
}

int winp::thread::object::run_(){
	MSG msg;
	m_callback_type sent_task;
	std::list<m_callback_type> sent_task_list;

	auto value = 0;
	auto task_was_run_ = true;

	windows_manager_.prepare_for_run_();
	get_all_sent_tasks_(sent_task_list);
	for (auto &task : sent_task_list)
		task();//Execute initially sent tasks

	while (!app::object::is_shut_down_ && !windows_manager_.toplevel_map_.empty()){
		if ((sent_task = get_next_sent_task_()) == nullptr){
			if (task_was_run_){//Check for possible message in queue. If none then execute task
				auto peek_status = PeekMessageW(&msg, nullptr, 0u, 0u, PM_NOREMOVE);
				if (peek_status == FALSE || msg.message == WM_TIMER || msg.message == WM_PAINT || msg.message == WM_NCPAINT || msg.message == WM_ERASEBKGND){
					task_was_run_ = run_task_();
					continue;
				}
			}
			else
				task_was_run_ = true;

			if (GetMessageW(&msg, nullptr, 0u, 0u) == -1 || app::object::is_shut_down_){
				value = -1;
				break;
			}

			if (msg.message == WM_QUIT){//Quit message posted
				value = static_cast<int>(msg.wParam);
				break;
			}
		}
		else//Execute sent task
			sent_task();
	}
	 
	is_exiting_ = true;
	while (!app::object::is_shut_down_ && run_task_()){}//Execute remaining tasks

	return value;
}

bool winp::thread::object::run_task_(){
	auto task = get_next_task_();
	if (task == nullptr)
		return false;

	task();

	return true;
}

void winp::thread::object::get_all_sent_tasks_(std::list<m_callback_type> &list){
	queue_.pop_all_send_priorities_(list);
}

winp::thread::object::m_callback_type winp::thread::object::get_next_sent_task_(){
	return queue_.pop_send_priority_();
}

winp::thread::object::m_callback_type winp::thread::object::get_next_task_(){
	return queue_.pop_();
}

void winp::thread::object::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(post_message)){
		static_cast<post_message *>(buf)->target_ = this;
		static_cast<post_message *>(buf)->init_();
	}
	else if (id == typeid(object *))
		*static_cast<object **>(buf) = this;
	else
		throw_(error_value_type::cannot_handle_request);
}

void winp::thread::object::throw_(error_value_type value) const{
	app::object::error = value;
}
