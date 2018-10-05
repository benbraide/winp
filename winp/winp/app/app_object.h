#pragma once

#include <atomic>
#include <memory>

#include "../thread/thread_object.h"
#include "../ui/ui_window_surface.h"

#define WINP_CLASS_UUID		 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define WINP_CLASS_WUUID	L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

namespace winp::app{
	class object{
	public:
		using m_thread_type = thread::object;

		static void init();

		static void shut_down();

		static int run(bool shut_down_after = true);

		static bool is_native_handle(HWND handle);

		static prop::list<std::list<m_thread_type *>, object, prop::proxy_value> threads;
		static prop::scalar<m_thread_type *, object, prop::proxy_value> current_thread;
		static prop::scalar<m_thread_type *, object, prop::proxy_value> main_thread;

		static prop::scalar<bool, object, prop::proxy_value> is_shut_down;
		static thread_local prop::error<object> error;

		static std::mutex lock;

	protected:
		friend class thread::object;
		friend class thread::surface_manager;

		static std::size_t add_thread_(m_thread_type &thread);

		static bool remove_thread_(m_thread_type &thread);

		static bool remove_thread_at_(std::size_t index);

		static std::size_t find_thread_(const m_thread_type &thread);

		static m_thread_type *get_thread_at_(std::size_t index);

		static std::shared_ptr<thread::object> main_thread_;
		static std::list<m_thread_type *> threads_;
		static std::atomic_bool is_shut_down_;

		static WNDCLASSEXW class_info_;
		static thread_local m_thread_type *current_thread_;
	};
}
