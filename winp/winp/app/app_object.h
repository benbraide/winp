#pragma once

#include "../thread/thread_object.h"

namespace winp::app{
	class collection;

	class object{
	public:
		using m_thread_type = thread::object;

		object();

		virtual ~object();

		void each(const std::function<bool(m_thread_type &)> &callback) const;

		m_thread_type *find(std::thread::id id) const;

		m_thread_type *find(DWORD id) const;

		static m_thread_type *find_main();

		prop::scalar<bool, object, prop::proxy_value> is_main;
		prop::error<object> error;

	protected:
		friend class collection;
		friend class thread::object;

		void add_(m_thread_type &target);

		void remove_(m_thread_type *target);

		bool is_main_;
		std::list<m_thread_type *> list_;
		mutable std::mutex lock_;

		static m_thread_type *main_;
	};
}
