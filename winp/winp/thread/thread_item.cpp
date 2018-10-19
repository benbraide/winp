#include "../app/app_object.h"

winp::thread::item::item()
	: thread_(nullptr){}

winp::thread::item::item(object &thread)
	: thread_(&thread){}

winp::thread::item::~item() = default;

const winp::thread::object &winp::thread::item::get_thread() const{
	return *thread_;
}

winp::thread::object &winp::thread::item::get_thread(){
	return *thread_;
}
