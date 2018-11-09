#include "app/app_object.h"

#include "utility/timer.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::ui::window_surface ws;
	ws.set_position(POINT{ 10, 10 });
	ws.set_size(SIZE{ 600, 400 });
	ws.set_styles(WS_OVERLAPPEDWINDOW, false);
	ws.create();
	ws.show(cmd_show);

	return winp::app::object::run();
}
