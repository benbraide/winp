#pragma once

#include <memory>

#include "../thread/thread_object.h"
#include "../property/list_property.h"

namespace winp::app{
	class object{
	public:
		using m_thread_type = thread::object;

		static void init();

		static void shut_down();

		static int run(bool shut_down_after = false);

		static prop::list<std::list<m_thread_type *>, object, prop::proxy_value> threads;
		static prop::scalar<m_thread_type *, object, prop::proxy_value> current_thread;
		static prop::scalar<m_thread_type *, object, prop::proxy_value> main_thread;

		static prop::scalar<bool, object, prop::proxy_value> is_shut_down;
		static prop::error<object> error;

	protected:
		friend class thread::object;

		static std::size_t add_thread_(m_thread_type &thread);

		static bool remove_thread_(m_thread_type &thread);

		static bool remove_thread_at_(std::size_t index);

		static std::size_t find_thread_(const m_thread_type &thread);

		static m_thread_type *get_thread_at_(std::size_t index);

		static m_thread_type *get_current_thread_();

		static std::list<m_thread_type *> threads_;
		static std::mutex lock_;
		static bool is_shut_down_;

		static std::shared_ptr<thread::object> main_thread_;
		static std::shared_ptr<object> dummy_app_;
	};
}
