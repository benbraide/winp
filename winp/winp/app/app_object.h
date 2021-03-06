#pragma once

#include <atomic>
#include <memory>
#include <unordered_map>

#include "../menu/menu_object.h"
#include "../thread/thread_object.h"
#include "../control/control_object.h"
#include "../message/message_dispatcher.h"
#include "../non_window/child_non_window.h"
#include "../ui/ui_grid_object.h"

#define WINP_CLASS_UUID		 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define WINP_CLASS_WUUID	L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

namespace winp::app{
	class object{
	public:
		using m_thread_type = thread::object;

		static void shut_down();

		static bool is_shut_down();

		static int run(bool shut_down_after = true);

		static WNDPROC get_default_message_entry(const wchar_t *class_name);

		static m_thread_type &get_main_thread();

		static thread_local m_thread_type this_thread;

	protected:
		friend class thread::item;
		friend class thread::object;
		friend class thread::surface_manager;

		static void init_();

		static void add_thread_(m_thread_type &thread);

		static void add_main_thread_(m_thread_type &thread);

		static void remove_thread_(DWORD id);

		static m_thread_type *find_thread_(DWORD id);

		static std::unordered_map<DWORD, m_thread_type *> threads_;
		static std::unordered_map<DWORD, std::shared_ptr<m_thread_type>> created_threads_;

		static bool is_shut_down_;
		static DWORD main_thread_id_;

		static WNDCLASSEXW class_info_;
		static std::unordered_map<std::size_t, WNDPROC> message_entry_list_;
		static std::mutex lock_;
	};
}
