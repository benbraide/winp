#include "../app/app_object.h"

winp::thread::object::object()
	: queue(*this){
	id_ = std::this_thread::get_id();
	local_id_ = GetCurrentThreadId();
	
	app::object::add_thread_(*this);
	surface_manager_.prepare_for_run_();
}

winp::thread::object::~object(){
	app::object::remove_thread_(local_id_);
}

int winp::thread::object::run(){
	if (!is_thread_context())
		return -1;

	MSG msg;
	m_callback_type sent_task;
	std::list<m_callback_type> sent_task_list;

	auto value = 0;
	auto task_was_run_ = true;

	message_hwnd_ = CreateWindowExW(0, app::object::class_info_.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
	get_all_sent_tasks_(sent_task_list);

	for (auto &task : sent_task_list)
		task();//Execute initially sent tasks

	is_exiting_ = false;
	while (!is_exiting_ && !surface_manager_.toplevel_map_.empty()){
		if ((sent_task = get_next_sent_task_()) == nullptr){
			if (task_was_run_){//Check for possible message in queue. If none then execute task
				auto peek_status = PeekMessageW(&msg, nullptr, 0u, 0u, PM_NOREMOVE);
				if (peek_status == FALSE || msg.message == WM_TIMER || msg.message == WM_PAINT || msg.message == WM_NCPAINT || msg.message == WM_ERASEBKGND){
					task_was_run_ = run_task_();
					continue;
				}
			}
			else
				task_was_run_ = true;

			if (is_exiting_ || GetMessageW(&msg, nullptr, 0u, 0u) == -1 || is_exiting_){
				value = -2;
				break;
			}

			if (msg.message == WM_QUIT){//Quit message posted
				value = static_cast<int>(msg.wParam);
				break;
			}

			surface_manager_.dispatch_message_(msg);
		}
		else//Execute sent task
			sent_task();
	}

	is_exiting_ = true;
	while (run_task_()){}//Execute remaining tasks

	return value;
}

void winp::thread::object::stop(){
	if (!is_thread_context()){
		queue.post([this]{
			is_exiting_ = true;
		}, thread::queue::send_priority);
	}
	else
		is_exiting_ = true;
}

bool winp::thread::object::is_main() const{
	return (local_id_ == app::object::main_thread_id_);
}

bool winp::thread::object::is_thread_context() const{
	return (GetCurrentThreadId() == local_id_);
}

std::thread::id winp::thread::object::get_id() const{
	return id_;
}

DWORD winp::thread::object::get_local_id() const{
	return local_id_;
}

ID2D1Factory *winp::thread::object::get_draw_factory() const{
	return (is_thread_context() ? get_draw_factory_() : nullptr);
}

IDWriteFactory *winp::thread::object::get_write_factory() const{
	return (is_thread_context() ? get_write_factory_() : nullptr);
}

ID2D1DCRenderTarget *winp::thread::object::get_device_drawer() const{
	return (is_thread_context() ? get_device_drawer_() : nullptr);
}

ID2D1SolidColorBrush *winp::thread::object::get_color_brush() const{
	return (is_thread_context() ? get_color_brush_() : nullptr);
}

ID2D1Factory *winp::thread::object::get_draw_factory_() const{
	if (draw_factory_ == nullptr)
		D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, &draw_factory_);
	return draw_factory_;
}

IDWriteFactory *winp::thread::object::get_write_factory_() const{
	if (write_factory_ == nullptr)
		DWriteCreateFactory(DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&write_factory_));
	return write_factory_;
}

ID2D1DCRenderTarget *winp::thread::object::get_device_drawer_() const{
	if (device_drawer_ == nullptr){
		auto draw_factory = get_draw_factory_();
		if (draw_factory != nullptr){
			auto props = D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					D2D1_ALPHA_MODE_PREMULTIPLIED
				),
				0,
				0,
				D2D1_RENDER_TARGET_USAGE_NONE,
				D2D1_FEATURE_LEVEL_DEFAULT
			);

			draw_factory->CreateDCRenderTarget(&props, &device_drawer_);
		}
	}

	return device_drawer_;
}

ID2D1SolidColorBrush *winp::thread::object::get_color_brush_() const{
	if (color_brush_ == nullptr){
		auto device_drawer = get_device_drawer_();
		if (device_drawer != nullptr){
			device_drawer->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
				D2D1::BrushProperties(),
				&color_brush_
			);
		}
	}

	return color_brush_;
}

void winp::thread::object::add_to_black_list_(unsigned __int64 id){
	queue.add_to_black_list_(id);
}

void winp::thread::object::remove_from_black_list_(unsigned __int64 id){
	queue.remove_from_black_list_(id);
}

bool winp::thread::object::run_task_(){
	auto task = get_next_task_();
	if (task == nullptr)
		return false;

	task();

	return true;
}

void winp::thread::object::run_all_tasks_(){
	while (run_task_()){}
}

void winp::thread::object::get_all_sent_tasks_(std::list<m_callback_type> &list){
	queue.pop_all_send_priorities_(list);
}

winp::thread::object::m_callback_type winp::thread::object::get_next_sent_task_(){
	return queue.pop_send_priority_();
}

winp::thread::object::m_callback_type winp::thread::object::get_next_task_(){
	return queue.pop_();
}
