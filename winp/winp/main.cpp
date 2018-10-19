#include "app/app_object.h"

#include "utility/timer.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::object::init();

	return 0;
}
