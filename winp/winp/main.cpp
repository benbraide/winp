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
		wm.insert<winp::menu::separator>([](winp::menu::separator &it){
			return true;
		});
		wm.insert<winp::menu::item>([](winp::menu::item &it){
			it.set_label(L"Last Item");
			return true;
		});
		wm.insert<winp::menu::check_item>([](winp::menu::check_item &it){
			it.set_label(L"Check Item");
			return true;
		});
		wm.insert<winp::menu::separator>([](winp::menu::separator &it){
			return true;
		});
		wm.insert<winp::menu::radio_group_collection>([](winp::menu::radio_group_collection &rg){
			rg.insert<winp::menu::check_item>([](winp::menu::check_item &it){
				it.set_label(L"Radio Item 1");
				return true;
			});
			rg.insert<winp::menu::check_item>([](winp::menu::check_item &it){
				it.set_label(L"Radio Item 2");
				return true;
			});
			rg.insert<winp::menu::check_item>([](winp::menu::check_item &it){
				it.set_label(L"Radio Item 3");
				return true;
			});
			return true;
		});
	});

	return winp::app::object::run();
}
