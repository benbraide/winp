#include "../app/app_object.h"

winp::thread::post_message::post_message() = default;

winp::thread::post_message::post_message(const post_message &copy)
	: target_(copy.target_){
	if (target_ != nullptr)
		init_();
}

winp::thread::post_message &winp::thread::post_message::operator=(const post_message &copy){
	if ((target_ = copy.target_) != nullptr && target.getter_ == nullptr)
		init_();
	return *this;
}

void winp::thread::post_message::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &target)
			*static_cast<object **>(buf) = target_;
		else if (&prop == &result)
			*static_cast<bool *>(buf) = (PostThreadMessageW(target_->id, code.m_value_, wparam.m_value_, lparam.m_value_) != FALSE);
	};

	target.init_(nullptr, nullptr, getter);
	result.init_(nullptr, nullptr, getter);
}

winp::thread::object::object(const std::function<void(object &)> &entry)
	: queue_(*this){
	init_();
	std::thread([=]{
		id_ = std::this_thread::get_id();
		local_id_ = GetCurrentThreadId();

		app::object::current_thread_ = this;
		if (entry != nullptr)
			entry(*this);

		run_();

		id_ = std::thread::id();
		local_id_ = 0u;
	}).detach();
}

winp::thread::object::object(bool)
	: queue_(*this){
	init_();
	id_ = std::this_thread::get_id();
	local_id_ = GetCurrentThreadId();
	app::object::current_thread_ = this;
}

winp::thread::object::~object(){
	app::object::threads -= this;
}

void winp::thread::object::init_(){
	auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
		if (&prop == &id){
			switch (context){
			case 0u:
				*static_cast<DWORD *>(buf) = local_id_;
				break;
			case 1u:
				*static_cast<std::thread::id *>(buf) = id_;
				break;
			default:
				break;
			}
		}
		else if (&prop == &queue)
			*static_cast<thread::queue **>(buf) = &queue_;
		else if (&prop == &draw_factory)
			*static_cast<ID2D1Factory **>(buf) = get_draw_factory_();
		else if (&prop == &write_factory)
			*static_cast<IDWriteFactory **>(buf) = get_write_factory_();
		else if (&prop == &is_main)
			*static_cast<bool *>(buf) = is_main_;
		else if (&prop == &inside)
			*static_cast<bool *>(buf) = (std::this_thread::get_id() == id_);
		else if (&prop == &request)
			do_request_(buf, *reinterpret_cast<std::type_info *>(context));
	};

	queue.init_(nullptr, nullptr, getter);
	id.init_(nullptr, nullptr, getter);
	request.init_(nullptr, nullptr, getter);

	is_main.init_(nullptr, nullptr, getter);
	inside.init_(nullptr, nullptr, getter);

	draw_factory.init_(nullptr, nullptr, getter);
	write_factory.init_(nullptr, nullptr, getter);

	app::object::threads += this;
}

int winp::thread::object::run_(){
	MSG msg;
	m_callback_type sent_task;
	std::list<m_callback_type> sent_task_list;

	auto value = 0;
	auto task_was_run_ = true;

	message_hwnd_ = CreateWindowExW(0, app::object::class_info_.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
	windows_manager_.prepare_for_run_();

	get_all_sent_tasks_(sent_task_list);
	for (auto &task : sent_task_list)
		task();//Execute initially sent tasks

	while (!app::object::is_shut_down_ && !windows_manager_.toplevel_map_.empty()){
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

			if (GetMessageW(&msg, nullptr, 0u, 0u) == -1 || app::object::is_shut_down_){
				value = -1;
				break;
			}

			if (msg.message == WM_QUIT){//Quit message posted
				value = static_cast<int>(msg.wParam);
				break;
			}
		}
		else//Execute sent task
			sent_task();
	}

	is_exiting_ = true;
	while (!app::object::is_shut_down_ && run_task_()){}//Execute remaining tasks

	return value;
}

bool winp::thread::object::run_task_(){
	auto task = get_next_task_();
	if (task == nullptr)
		return false;

	task();

	return true;
}

void winp::thread::object::get_all_sent_tasks_(std::list<m_callback_type> &list){
	queue_.pop_all_send_priorities_(list);
}

winp::thread::object::m_callback_type winp::thread::object::get_next_sent_task_(){
	return queue_.pop_send_priority_();
}

winp::thread::object::m_callback_type winp::thread::object::get_next_task_(){
	return queue_.pop_();
}

ID2D1Factory *winp::thread::object::get_draw_factory_(){
	if (draw_factory_ == nullptr)//Create factory
		D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, &draw_factory_);
	return draw_factory_;
}

IDWriteFactory *winp::thread::object::get_write_factory_(){
	if (write_factory_ == nullptr)//Create factory
		DWriteCreateFactory(DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&write_factory_));
	return write_factory_;
}

ID2D1DCRenderTarget *winp::thread::object::get_device_render_(){
	if (device_render_ == nullptr){
		auto factory = get_draw_factory_();
		if (factory != nullptr){
			auto props = D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					D2D1_ALPHA_MODE_PREMULTIPLIED),
				0,
				0,
				D2D1_RENDER_TARGET_USAGE_NONE,
				D2D1_FEATURE_LEVEL_DEFAULT
			);

			factory->CreateDCRenderTarget(&props, &device_render_);
		}
	}

	return device_render_;
}

ID2D1SolidColorBrush *winp::thread::object::get_color_brush_(){
	if (color_brush_ == nullptr){
		auto render = get_device_render_();
		if (render != nullptr){
			render->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
				D2D1::BrushProperties(),
				&color_brush_
			);
		}
	}

	return color_brush_;
}

void winp::thread::object::do_request_(void *buf, const std::type_info &id){
	if (id == typeid(post_message)){
		static_cast<post_message *>(buf)->target_ = this;
		static_cast<post_message *>(buf)->init_();
	}
	else if (id == typeid(object *))
		*static_cast<object **>(buf) = this;
	else
		throw_(error_value_type::cannot_handle_request);
}

void winp::thread::object::throw_(error_value_type value) const{
	app::object::error = value;
}
