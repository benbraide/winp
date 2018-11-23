#include "app/app_object.h"

#include "window/frame_window.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::window::frame ws;
	ws.set_position(POINT{ 10, 10 });
	ws.set_size(SIZE{ 600, 400 });
	ws.set_styles(WS_OVERLAPPEDWINDOW, false);
	ws.set_caption(L"Framed Window");
	ws.create();
	ws.show(cmd_show);
	ws.get_system_menu([](winp::menu::wrapper_collection &wm){
		wm.insert_separator();
		wm.insert_item(L"Last Item");
		wm.insert_check_item(L"Check Item");
		wm.insert_separator();
		wm.insert<winp::menu::radio_group_collection>([](winp::menu::radio_group_collection &rg){
			rg.insert_check_item(L"Radio Item 1");
			rg.insert_check_item(L"Radio Item 2");
			rg.insert_check_item(L"Radio Item 3");
			return true;
		});
	});

	return winp::app::object::run();
}
