#include "../app/app_object.h"

winp::thread::item::item()
	: thread_(app::object::this_thread){
	id_ = reinterpret_cast<std::size_t>(this);
}

winp::thread::item::item(object &thread)
	: thread_(thread){
	id_ = reinterpret_cast<std::size_t>(this);
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
	execute_using_context([this]{
		if (!destructed_){
			destructed_ = true;
			destruct_();
		}
	});
}

void winp::thread::item::post_task(const queue::callback_type &task, int priority) const{
	thread_.queue.post(task, priority, id_);
}

void winp::thread::item::use_context(const queue::callback_type &task, int priority) const{
	if (thread_.is_thread_context())
		task();
	else
		thread_.queue.post(task, priority, id_);
}

bool winp::thread::item::is_thread_context() const{
	return thread_.is_thread_context();
}

unsigned __int64 winp::thread::item::get_id() const{
	return id_;
}

void winp::thread::item::event_handlers_count_changed_(event::manager_base &e, std::size_t previous_count, std::size_t current_count){}

void winp::thread::item::destruct_(){
	thread_.add_to_black_list_(id_);
}

winp::thread::queue &winp::thread::item::get_queue_() const{
	return thread_.queue;
}
