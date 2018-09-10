#pragma once

#include "../message/message_object.h"
#include "../property/variant_property.h"

namespace winp::app{
	class object;
}

namespace winp::thread{
	class object;
	class item;

	class item_message{
	public:
		enum class action_type{
			send,
			post,
		};

		item_message();

		item_message(const item_message &copy);

		item_message &operator =(const item_message &copy);

		LRESULT send();

		bool post();

		prop::scalar<HWND, item_message, prop::proxy_value> target;
		prop::scalar<LRESULT, item_message, prop::proxy_value> result;
		prop::scalar<action_type, item_message, prop::proxy_value> action;

		prop::scalar<unsigned int, item_message, prop::immediate_value> code;
		prop::scalar<WPARAM, item_message, prop::immediate_value> wparam;
		prop::scalar<LPARAM, item_message, prop::immediate_value> lparam;

	private:
		friend class item;

		void init_();

		HWND target_ = nullptr;
		LRESULT result_ = 0;
	};

	class item{
	public:
		using m_message_type = message::object;

		explicit item(object &owner);

		virtual ~item();

		prop::scalar<object *, item, prop::proxy_value> thread;
		prop::scalar<HWND, item, prop::proxy_value> handle;
		prop::variant<item> request;

	protected:
		friend class object;
		friend class app::object;
		friend class item_message;

		item();

		void init_();

		virtual void destroy_();

		virtual void do_request_(void *buf, const std::type_info &id);

		virtual HWND get_handle_() const;

		object *owner_;
		HWND handle_;
	};
}
