#include "../app/app_object.h"

winp::thread::item::item()
	: thread_(*app::object::get_or_create_thread()), id_(0u){
	id_ = thread_.random_generator_(1ui64, std::numeric_limits<unsigned __int64>::max());
	thread_.remove_from_black_list_(id_);//In case ID is reused
}

winp::thread::item::item(object &thread)
	: thread_(thread), id_(thread.random_generator_(1ui64, std::numeric_limits<unsigned __int64>::max())){
	thread_.remove_from_black_list_(id_);//In case ID is reused
}

winp::thread::item::~item(){
	destruct();
}

const winp::thread::object &winp::thread::item::get_thread() const{
	return thread_;
}

winp::thread::object &winp::thread::item::get_thread(){
	return thread_;
}

void winp::thread::item::destruct(){
	static bool called = false;
	if (called)//Already called
		return;

	if (!thread_.is_thread_context()){
		thread_.queue.add([=]{
			destruct_();
		}, thread::queue::send_priority, id_).get();
	}
	else//Inside thread context
		destruct_();

	called = true;
}

void winp::thread::item::use_context(const queue::callback_type &task, int priority){
	if (thread_.is_thread_context())
		task();
	else
		thread_.queue.post(task, priority, id_);
}

void winp::thread::item::destruct_(){
	thread_.add_to_black_list_(id_);
}
