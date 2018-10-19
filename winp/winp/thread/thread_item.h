#pragma once

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

		explicit item(object &thread);

		virtual ~item();

		virtual const object &get_thread() const;

		virtual object &get_thread();

	protected:
		friend class object;
		friend class app::object;

		item();

		object *thread_;
	};
}
