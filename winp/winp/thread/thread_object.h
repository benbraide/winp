#pragma once

#include <memory>
#include <unordered_map>

#include "../property/error_property.h"
#include "../property/variant_property.h"
#include "../property/map_property.h"

#include "thread_queue.h"
#include "surface_manager.h"

namespace winp::app{
	class object;
}

namespace winp::message{
	class object;
}

namespace winp::ui{
	class surface;
	class window_surface;
}

namespace winp::thread{
	class object;

	class post_message{
	public:
		post_message();

		post_message(const post_message &copy);

		post_message &operator =(const post_message &copy);

		prop::scalar<object *, post_message, prop::proxy_value> target;
		prop::scalar<bool, post_message, prop::proxy_value> result;

		prop::scalar<unsigned int, post_message, prop::immediate_value> code;
		prop::scalar<WPARAM, post_message, prop::immediate_value> wparam;
		prop::scalar<LPARAM, post_message, prop::immediate_value> lparam;

	private:
		friend class object;

		void init_();

		object *target_ = nullptr;
	};

	class object{
	public:
		using queue_type = queue;
		using m_callback_type = queue::callback_type;
		using m_app_type = app::object;
		using error_value_type = prop::default_error_mapper::value_type;

		object();

		virtual ~object();

		prop::scalar<bool, object, prop::proxy_value> is_main;
		prop::scalar<bool, object, prop::proxy_value> inside;

		prop::variant<object, prop::proxy_value, std::thread::id, DWORD> id;
		prop::scalar<queue *, object> queue;

		prop::variant<object, prop::proxy_value> request;

	protected:
		friend class app::object;
		friend class message::object;

		friend class item;
		friend class post_message;

		friend class ui::surface;
		friend class ui::window_surface;
		friend class surface_manager;

		explicit object(bool);

		void init_();

		virtual int run_();

		virtual bool run_task_();

		virtual void get_all_sent_tasks_(std::list<m_callback_type> &list);

		virtual m_callback_type get_next_sent_task_();

		virtual m_callback_type get_next_task_();

		virtual void do_request_(void *buf, const std::type_info &id);

		virtual void throw_(error_value_type value) const;

		queue_type queue_;
		std::thread::id id_;
		DWORD local_id_ = 0;

		bool is_main_;
		bool is_exiting_;
		surface_manager windows_manager_;
	};
}
