#include "app/app_object.h"

#include "thread/thread_object.h"
#include "thread/thread_value.h"

#include "property/pair_property.h"
#include "property/quad_property.h"

#include "utility/timer.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	using timer_type = winp::utility::timer<>;

	winp::app::object::init();

	winp::thread::item thit(*winp::app::object::main_thread);
	winp::thread::item_message itmsg = thit.request;

	timer_type timer(*winp::app::object::main_thread);
	timer_type::interval_type int_timer = timer.request;
	timer_type::counter_type cnt_timer = timer.request;

	cnt_timer.running = true;
	std::this_thread::sleep_for(std::chrono::nanoseconds(5000000000));
	timer_type::counter_type::m_precision prec = cnt_timer.elapsed;
	prec = cnt_timer.elapsed;

	winp::prop::scalar<std::shared_ptr<int>> sptr;
	sptr = std::make_shared<int>(99);

	winp::prop::size<void, int, std::string, winp::prop::immediate_value> sz;
	sz.height = "18";
	sz.width = 45;
	sz = { 9, "72" };
	sz = winp::utility::size<int, std::string>{ 9, "72" };

	winp::utility::size<int, std::string> csz = sz;
	std::string fv = sz.height;
	int sv = sz.width;
	winp::prop::size<void, int, std::string, winp::prop::immediate_value>::m_value_type szv = sz;

	winp::prop::rect<void, int, int, int, int, winp::prop::immediate_value> rct;
	rct.left = 9;
	rct.top = 18;
	rct.right = 27;
	rct.bottom = 36;
	rct = { 45, 54, 63, 72 };

	int frv = rct.left;
	int srv = rct.top;
	int trv = rct.right;
	int lrv = rct.bottom;
	winp::prop::rect<void, int, int, int, int, winp::prop::immediate_value>::m_value_type rv = rct;

	auto ils = { 9, 18 };
	std::tuple<int, int> tpl = { 9, 18 };
	auto tpli1 = std::get<0>(tpl);
	auto tpli2 = std::get<1>(tpl);

	/*auto fut = winp::thread::object::main_->queue->add([]{
		return 9;
	});

	auto v = fut.get();*/

	auto i = 9;
	auto p = &i;
	auto w = CreateWindowW(L"BUTTON", L"", 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, 0);

	auto l1 = static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(p));
	auto l2 = static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(w));

	auto h1 = static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(p) >> 32);
	auto h2 = static_cast<unsigned __int32>(reinterpret_cast<unsigned __int64>(w) >> 32);

	winp::thread::typed_value<int> tiv(45);
	winp::thread::typed_value<std::wstring> tsv(L"Test");

	auto ltiv = tiv.get_local();
	auto itiv = tiv.get<int>();
	auto btiv = tiv.get<bool>();

	auto ltsv = tsv.get_local();
	auto itsv = tsv.get<int>();
	auto btsv = tsv.get<bool>();
	auto stsv = tsv.get<std::wstring>();
	auto &srtsv = tsv.get<std::wstring &>();
	auto sptsv = tsv.get<std::wstring *>();
	auto sprtsv = tsv.get_ref<std::wstring>();

	//High order is zero for hwnd

	return 0;
}
