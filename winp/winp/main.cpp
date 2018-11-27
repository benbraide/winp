#include "app/app_object.h"

#include "window/frame_window.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::window::frame ws;
	ws.set_position(POINT{ 10, 10 });
	ws.set_size(SIZE{ 600, 400 });
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
		wm.insert_separator();
		wm.insert_link(L"Link Item", [](winp::menu::collection &popup){
			popup.insert_item(L"Popup Item");
			popup.insert_check_item(L"Popup Check Item");
			popup.insert_link(L"Pushed Link Item", [](winp::menu::collection &popup2){
				popup2.draw_item_event += [](winp::event::draw_item &e){};
				popup2.insert_item(L"Popup2 Item", [](winp::menu::item &odi){
					return true;
				});
				popup2.insert_link(L"Link Item", [](winp::menu::collection &popup3){
					popup3.insert_item(L"Popup3 Item", [](winp::menu::item &odi){
						return true;
					});
					return true;
				});
				popup2.insert_check_item(L"Popup2 Check Item", [](winp::menu::item &odi){
					return true;
				});
				popup2.insert_item(L"Owner Drawn Item", [](winp::menu::item &odi){
					return true;
				});
				return true;
			}, 0);
			return true;
		});
	});

	return winp::app::object::run();
}
