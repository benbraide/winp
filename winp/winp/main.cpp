#include "thread/thread_object.h"
#include "thread/thread_value.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
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
