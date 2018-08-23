#pragma once

#include "../message/message_object.h"

namespace winp::app{
	class object;
}

namespace winp::thread{
	class object;

	class item{
	public:
		using m_message_type = message::object;

		explicit item(object &owner);

		virtual ~item();

		prop::scalar<object *, item, prop::proxy_value> owner;
		prop::scalar<HWND, item, prop::proxy_value> handle;

	protected:
		friend class object;
		friend class app::object;

		item();

		object *owner_;
		HWND handle_;
	};
}
