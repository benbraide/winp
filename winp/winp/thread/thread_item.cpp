#include "../app/app_object.h"

winp::thread::item::item()
	: thread_(nullptr), id_(0u){}

winp::thread::item::item(object &thread)
	: thread_(&thread), id_(thread.random_generator_(1ui64, std::numeric_limits<unsigned __int64>::max())){}

winp::thread::item::~item(){
	if (thread_ != nullptr)
		thread_->add_to_black_list_(id_);
}

const winp::thread::object &winp::thread::item::get_thread() const{
	return *thread_;
}

winp::thread::object &winp::thread::item::get_thread(){
	return *thread_;
}

void winp::thread::item::use_context(const queue::callback_type &task, int priority){
	thread_->queue.post(task, priority, id_);
}
