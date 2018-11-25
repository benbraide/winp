#include "../app/app_object.h"
#include "timer.h"

winp::utility::timer_object::timer_object()
	: thread_(app::object::this_thread){}

winp::utility::timer_object::timer_object(thread::object &thread)
	: thread_(thread){}

winp::utility::timer_object::~timer_object() = default;
