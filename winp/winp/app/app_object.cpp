#include "app_object.h"

void winp::app::object::init(){
	std::lock_guard<std::mutex> guard(lock_);
	init_();
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
	{//Scoped
		std::lock_guard<std::mutex> guard(lock_);
		if (is_shut_down_ || !is_initialized_ || GetCurrentThreadId() != main_thread_id_)
			return -1;
	}
	
	auto result = find_thread_(main_thread_id_)->run();
	if (shut_down_after)
		shut_down();

	return result;
}

winp::app::object::m_thread_type *winp::app::object::get_main_thread(){
	std::lock_guard<std::mutex> guard(lock_);
	return find_thread_(main_thread_id_);
}

winp::app::object::m_thread_type *winp::app::object::get_current_thread(){
	std::lock_guard<std::mutex> guard(lock_);
	return find_thread_(GetCurrentThreadId());
}

winp::app::object::m_thread_type *winp::app::object::get_or_create_thread(){
	std::lock_guard<std::mutex> guard(lock_);

	auto current_id = GetCurrentThreadId();
	auto thread = find_thread_(current_id);
	if (thread != nullptr)
		return thread;

	if (current_id == main_thread_id_){//App has not been initialized
		init_();
		return find_thread_(main_thread_id_);
	}

	auto created_thread = std::make_shared<m_thread_type>();
	if (created_thread == nullptr)
		return nullptr;

	threads_[current_id] = created_thread.get();
	created_threads_[current_id] = created_thread;

	return created_thread.get();
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

void winp::app::object::init_(){
	if (is_initialized_)
		return;//Already initialized

	is_initialized_ = true;
	std::shared_ptr<m_thread_type> main_thread;

	main_thread.reset(new thread::object(true));
	created_threads_[main_thread_id_] = main_thread;

	class_info_.cbSize = sizeof(WNDCLASSEXW);
	class_info_.hInstance = GetModuleHandleW(nullptr);
	class_info_.lpfnWndProc = thread::surface_manager::entry_;
	class_info_.lpszClassName = WINP_CLASS_WUUID;
	class_info_.lpszMenuName = nullptr;
	class_info_.style = (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
	class_info_.hbrBackground = nullptr;
	class_info_.hCursor = LoadCursorW(nullptr, IDC_ARROW);
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

void winp::app::object::add_thread_(m_thread_type &thread){
	std::lock_guard<std::mutex> guard(lock_);
	if (!is_shut_down_)
		threads_[thread.local_id_] = &thread;
}

void winp::app::object::add_main_thread_(m_thread_type &thread){
	if (!is_shut_down_){
		main_thread_id_ = GetCurrentThreadId();
		threads_[thread.local_id_] = &thread;
	}
}

void winp::app::object::remove_thread_(DWORD id){
	std::lock_guard<std::mutex> guard(lock_);
	if (!is_shut_down_){
		threads_.erase(id);
		created_threads_.erase(id);
	}
}

winp::app::object::m_thread_type *winp::app::object::find_thread_(DWORD id){
	if (is_shut_down_)
		return nullptr;

	auto it = threads_.find(id);
	return ((it == threads_.end()) ? nullptr : it->second);
}

std::unordered_map<DWORD, winp::app::object::m_thread_type *> winp::app::object::threads_;

std::unordered_map<DWORD, std::shared_ptr<winp::app::object::m_thread_type>> winp::app::object::created_threads_;

bool winp::app::object::is_initialized_ = false;

bool winp::app::object::is_shut_down_ = false;

WNDCLASSEXW winp::app::object::class_info_{};

std::unordered_map<std::size_t, WNDPROC> winp::app::object::message_entry_list_;

std::mutex winp::app::object::lock_;

DWORD winp::app::object::main_thread_id_ = GetCurrentThreadId();
