#include "thread_object.h"

winp::thread::object::object()
	: queue(&queue_), queue_(*this), is_main_(false){
	init_();
	std::thread([this]{
		id_ = std::this_thread::get_id();
		raw_id_ = GetCurrentThreadId();

		run_();

		id_ = std::thread::id();
		raw_id_ = 0u;
	}).detach();
}

winp::thread::object::object(bool)
	: queue(&queue_), queue_(*this), is_main_(true){
	id_ = std::this_thread::get_id();
	raw_id_ = GetCurrentThreadId();
}

winp::thread::object::~object() = default;

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
				*static_cast<DWORD *>(buf) = raw_id_;
				break;
			}
		}
		else if (&prop == &inside)
			*static_cast<bool *>(buf) = (std::this_thread::get_id() == id_);
		else if (&prop == &state)
			*static_cast<state_type *>(buf) = state_;
	};

	id.init_(*this, nullptr, nullptr, getter);
	state.init_(*this, nullptr, setter, getter);
	inside.init_(*this, nullptr, nullptr, getter);
}

void winp::thread::object::run_(){
	state_ = state_type::running;
	while (run_state_())
		run_task_();

	state_ = state_type::stopped;
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

winp::prop::default_error_mapper::value_type winp::thread::object::thread_context_mismatch = prop::default_error_mapper::value(L"Action can only be performed inside originating thread context.");

const winp::thread::object *winp::thread::object::main_object = new winp::thread::object(true);
