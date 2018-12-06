#pragma once

#include "../utility/windows.h"

#include "thread_queue.h"

namespace winp::app{
	class object;
}

namespace winp::event{
	class manager_base;
}

namespace winp::thread{
	class object;

	class item{
	public:
		item();

		explicit item(object &thread);

		virtual ~item();

		virtual const object &get_thread() const;

		virtual object &get_thread();

		virtual void destruct();

		virtual void use_context(const queue::callback_type &task, int priority = queue::send_priority) const;

		template <typename function_type>
		auto execute_using_context(const function_type &task, int priority = queue::send_priority) const{
			return get_queue_().execute(task, priority, id_);
		}

		virtual bool is_thread_context() const;

		virtual unsigned __int64 get_id() const;

	protected:
		friend class object;
		friend class app::object;
		friend class event::manager_base;

		virtual void destruct_();

		virtual void event_handlers_count_changed_(event::manager_base &e, std::size_t previous_count, std::size_t current_count);

		virtual queue &get_queue_() const;

		object &thread_;
		unsigned __int64 id_;
		bool destructed_ = false;
	};
}
