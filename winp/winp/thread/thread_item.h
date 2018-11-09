#pragma once

#include <exception>

#include "../message/message_object.h"

#include "thread_queue.h"

namespace winp::app{
	class object;
}

namespace winp::thread{
	class object;

	class item{
	public:
		using m_message_type = message::object;

		item();

		explicit item(object &thread);

		virtual ~item();

		virtual const object &get_thread() const;

		virtual object &get_thread();

		virtual void use_context(const queue::callback_type &task, int priority = queue::send_priority);

	protected:
		friend class object;
		friend class app::object;

		object *thread_;
		unsigned __int64 id_;
	};
}
