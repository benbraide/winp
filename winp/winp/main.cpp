#include "thread/thread_object.h"

//ReSharper disable once CppInconsistentNaming
int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	auto fut = winp::thread::object::main_object->queue->add([]{
		return 9;
	});

	auto v = fut.get();

	return 0;
}
