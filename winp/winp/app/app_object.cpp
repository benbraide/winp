#include "app_object.h"

void winp::app::object::init(){
	if (main_thread_ != nullptr)
		return;//Already initialized

	main_thread_.reset(new thread::object(true));
	thread::surface_manager::init_dispatchers_();

	class_info_.cbSize = sizeof(WNDCLASSEXW);
	class_info_.hInstance = GetModuleHandleW(nullptr);
	class_info_.lpfnWndProc = thread::surface_manager::entry_;
	class_info_.lpszClassName = WINP_CLASS_WUUID;
	class_info_.lpszMenuName = nullptr;
	class_info_.style = (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
	class_info_.hbrBackground = nullptr;
	class_info_.hCursor = nullptr;
	class_info_.hIconSm = nullptr;
	class_info_.hIcon = nullptr;
	class_info_.cbWndExtra = 0;
	class_info_.cbClsExtra = 0;

	RegisterClassExW(&class_info_);
	message_entry_list_[std::hash_value(WINP_CLASS_WUUID)] = DefWindowProcW;

	INITCOMMONCONTROLSEX info{
		sizeof(INITCOMMONCONTROLSEX),
		(ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES | ICC_USEREX_CLASSES | ICC_LINK_CLASS | ICC_DATE_CLASSES | ICC_COOL_CLASSES | ICC_ANIMATE_CLASS)
	};
	InitCommonControlsEx(&info);
}

void winp::app::object::shut_down(){
	std::lock_guard<std::mutex> guard(lock_);
	if (!is_shut_down_){
		is_shut_down_ = true;
		for (auto &thread : threads_)//Wake all sleeping threads
			thread.second->post_message(0u);
	}
}

bool winp::app::object::is_shut_down(){
	std::lock_guard<std::mutex> guard(lock_);
	return is_shut_down_;
}

int winp::app::object::run(bool shut_down_after){
	if (main_thread_ == nullptr || !main_thread_->is_thread_context() || is_shut_down())
		return -1;

	auto result = main_thread_->run();
	if (shut_down_after)
		shut_down();

	return result;
}

winp::app::object::m_thread_type *winp::app::object::get_main_thread(){
	return main_thread_.get();
}

winp::app::object::m_thread_type *winp::app::object::get_current_thread(){
	std::lock_guard<std::mutex> guard(lock_);
	if (is_shut_down_)
		return nullptr;

	auto it = threads_.find(GetCurrentThreadId());
	return ((it == threads_.end()) ? nullptr : it->second);
}

WNDPROC winp::app::object::get_default_message_entry(const wchar_t *class_name){
	std::lock_guard<std::mutex> guard(lock_);

	auto key = std::hash_value(class_name);
	auto it = message_entry_list_.find(key);
	if (it != message_entry_list_.end())
		return it->second;

	WNDCLASSEXW class_info{ sizeof(WNDCLASSEXW) };
	if (GetClassInfoExW(nullptr, class_name, &class_info) == FALSE)
		return nullptr;

	return (message_entry_list_[key] = class_info.lpfnWndProc);
}

void winp::app::object::add_thread_(m_thread_type &thread){
	std::lock_guard<std::mutex> guard(lock_);
	if (!is_shut_down_)
		threads_[thread.local_id_] = &thread;
}

void winp::app::object::remove_thread_(m_thread_type &thread){
	std::lock_guard<std::mutex> guard(lock_);
	if (!is_shut_down_)
		threads_.erase(thread.local_id_);
}

std::shared_ptr<winp::thread::object> winp::app::object::main_thread_;

std::unordered_map<DWORD, winp::app::object::m_thread_type *> winp::app::object::threads_;

bool winp::app::object::is_shut_down_ = false;

WNDCLASSEXW winp::app::object::class_info_{};

std::unordered_map<std::size_t, WNDPROC> winp::app::object::message_entry_list_;

std::mutex winp::app::object::lock_;
