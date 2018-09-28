#pragma once

#include "../message/message_object.h"
#include "../property/variant_property.h"

#include "thread_queue.h"

namespace winp::app{
	class object;
}

namespace winp::thread{
	class object;

	class item{
	public:
		using m_message_type = message::object;
		using error_value_type = prop::default_error_mapper::value_type;

		explicit item(object &owner);

		virtual ~item();

		prop::scalar<object *, item, prop::proxy_value> thread;
		prop::scalar<HWND, item, prop::proxy_value> handle;

		prop::variant<item, prop::proxy_value> request;
		prop::variant<item, prop::proxy_value> apply;

		prop::scalar<queue::callback_type, item, prop::proxy_value> queued_task;

	protected:
		friend class object;
		friend class app::object;

		item();

		void init_();

		virtual void destroy_();

		virtual void do_request_(void *buf, const std::type_info &id);

		virtual void do_apply_(const void *value, const std::type_info &id);

		virtual HWND get_handle_() const;

		virtual void throw_(error_value_type value) const;

		object *owner_;
		HWND handle_;
	};
}
