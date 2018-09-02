#pragma once

#include <memory>

#include "../property/error_property.h"
#include "../property/variant_property.h"

#include "thread_queue.h"
#include "thread_value_manager.h"
#include "thread_item.h"

namespace winp::app{
	class object;
}

namespace winp::message{
	class object;
}

namespace winp::thread{
	class object{
	public:
		using queue_type = queue;
		using m_callback_type = queue::callback_type;
		using m_app_type = app::object;

		template <class target_type>
		struct is_basic_value{
			static constexpr bool value = (
				   std::is_same_v<target_type, bool>
				|| std::is_pointer_v<target_type>
				|| std::is_integral_v<target_type>
			);
		};

		struct item_placeholders_type{
			std::shared_ptr<item> allocated_wparam_only;
			std::shared_ptr<item> allocated_lparam_only;
			std::shared_ptr<item> allocated_both;
		};

		object();

		virtual ~object();

		void each(const std::function<bool(item &)> &callback) const;

		template <typename return_type, typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		return_type send_to(HWND receiver, unsigned int msg, const wparam_type &wparam = wparam_type(), const lparam_type &lparam = lparam_type()) const{
			return send_to(reinterpret_cast<item *>(receiver), msg, wparam, lparam);
		}

		template <typename return_type, typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		return_type send_to(const item *receiver, unsigned int msg, const wparam_type &wparam = wparam_type(), const lparam_type &lparam = lparam_type()) const{
			return send_to1_(receiver, msg, wparam, lparam, std::bool_constant<is_basic_value<wparam_type>::value>());
		}

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		void post_to(const item *receiver, unsigned int msg, const wparam_type &wparam = wparam_type(), const lparam_type &lparam = lparam_type()) const{

		}

		prop::scalar<bool, object, prop::proxy_value> is_main;
		prop::scalar<bool, object, prop::proxy_value> inside;

		prop::variant<object, std::thread::id, DWORD> id;
		prop::scalar<queue *, object> queue;

	protected:
		friend class app::object;
		friend class item;
		friend class message::object;

		explicit object(bool);

		void add_(item &target);

		void remove_(item *target);

		void init_();

		virtual int run_();

		virtual bool run_task_();

		virtual m_callback_type get_next_task_();

		virtual void before_task_(m_callback_type &task);

		virtual void after_task_(m_callback_type &task);

		queue_type queue_;
		std::thread::id id_;
		DWORD local_id_ = 0;

		bool is_main_;
		bool is_exiting_;

		std::list<item *> list_;
	};
}
