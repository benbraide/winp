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

		virtual void use_context(const queue::callback_type &task, int priority = queue::send_priority);

	protected:
		friend class object;
		friend class app::object;
		friend class event::manager_base;

		virtual void destruct_();

		object &thread_;
		unsigned __int64 id_;
	};
}
