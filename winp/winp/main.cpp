#include "app/app_object.h"

#include "window/frame_window.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::window::frame ws;
	ws.close_event += [](winp::event::object &e){
		//e.prevent_default();
	};

	ws.create_event += [&ws]() mutable {
		std::thread([&ws]{
			std::this_thread::sleep_for(std::chrono::seconds(3));
			for (auto step = 1, count = 0; ; count += step){
				if (count == 20)
					step = -1;
				else if (count == 0)
					step = 1;
				//ws.offset_position(10 * step, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}).detach();
	};

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
		});
		wm.insert_separator();
		wm.insert_link(L"Link Item", [](winp::menu::collection &popup){
			popup.insert_item(L"Popup Item");
			popup.insert_check_item(L"Popup Check Item");
			popup.insert_link(L"Pushed Link Item", [](winp::menu::collection &popup2){
				popup2.draw_item_event += [](winp::event::draw_item &e){};
				popup2.insert_item(L"Popup2 Item");
				popup2.insert_link(L"Link Item", [](winp::menu::collection &popup3){
					popup3.insert_item(L"Popup3 Item");
				});
				popup2.insert_check_item(L"Popup2 Check Item");
				popup2.insert_item(L"Owner Drawn Item");
			}, 0);
		});
	});

	winp::non_window::child nwc(ws);
	nwc.draw_event += [](winp::event::draw &e){
		if (auto drawer = e.get_drawer(); drawer != nullptr){
			auto size = dynamic_cast<winp::ui::surface *>(e.get_context())->get_size();
			for (auto step = 10; step < size.cx; step += 10)
				drawer->DrawLine(D2D1::Point2F((float)step, 0.f), D2D1::Point2F((float)step, (float)size.cy), e.get_color_brush());
			for (auto step = 10; step < size.cy; step += 10)
				drawer->DrawLine(D2D1::Point2F(0.f, (float)step), D2D1::Point2F((float)size.cx, (float)step), e.get_color_brush());
		}
	};

	std::thread([&nwc]{
		std::this_thread::sleep_for(std::chrono::seconds(3));
		for (auto step = 1, count = 0; ; count += step){
			if (count == 20)
				step = -1;
			else if (count == 0)
				step = 1;
			//nwc.offset_position(10 * step, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}).detach();

	nwc.set_position(POINT{ 10, 10 });
	nwc.set_size(SIZE{ 300, 200 });
	nwc.set_background_color(D2D1::ColorF(D2D1::ColorF::Red));
	nwc.set_border_type(winp::non_window::child::border_type::ellipse);
	nwc.create();

	winp::non_window::child nwc2(nwc);
	//nwc2.set_position(POINT{ 50, 50 });
	nwc2.set_size(SIZE{ 90, 50 });
	nwc2.set_background_color(D2D1::ColorF(D2D1::ColorF::Green));
	nwc2.set_border_type(winp::non_window::child::border_type::round_rect);
	nwc2.set_border_curve_size(SIZE{ 10, 10 });
	nwc2.create();//Made non-window objects use region as their surface
	nwc2.set_hook<winp::ui::alignment_modifier>()->set_modifier_alignment(winp::ui::alignment_modifier::align_center | winp::ui::alignment_modifier::align_vcenter);

	return winp::app::object::run();
}
