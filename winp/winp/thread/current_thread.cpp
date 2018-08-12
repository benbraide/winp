#include "../app/app_collection.h"

#include "current_thread.h"

winp::thread::object *winp::thread::current::get_object(){
	object *found = nullptr;
	app::collection::each([&found](app::object &object){
		if ((found = object.find(0)) != nullptr)
			return false;
		return true;
	});

	return found;
}

std::thread::id winp::thread::current::get_id(){
	return std::this_thread::get_id();
}

DWORD winp::thread::current::get_local_id(){
	return GetCurrentThreadId();
}
