#pragma once

#include "../message/message_object.h"

namespace winp::thread{
	class object;

	class item{
	public:
		using m_message_type = message::object;

		item();

		virtual ~item();

		prop::scalar<object *, item, prop::proxy_value> owner;
		prop::scalar<HWND, item, prop::proxy_value> handle;

		prop::error<item> error;

	protected:
		friend class object;

		virtual void change_owner_(object *value);

		object *owner_;
		HWND handle_;
	};
}
