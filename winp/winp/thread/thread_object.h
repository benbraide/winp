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
	enum class state_type{
		nil,
		stopped,
		running,
		suspended,
	};

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

		prop::variant<object, std::thread::id, DWORD> id;
		prop::scalar<queue *, object> queue;

		prop::scalar<state_type, object, prop::proxy_value> state;
		prop::scalar<bool, object, prop::proxy_value> inside;

		static const item_placeholders_type item_placeholders;

	protected:
		friend class app::object;
		friend class item;
		friend class message::object;

		explicit object(bool);

		void add_(item &target);

		void remove_(item *target);

		void init_();

		virtual int run_();

		virtual bool run_state_() const;

		virtual void run_task_();

		virtual m_callback_type get_next_task_();

		virtual void before_task_();

		virtual void after_task_();

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to1_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::true_type) const{
			return send_to1b_(receiver, msg, wparam, lparam, std::bool_constant<std::is_same_v<wparam_type, bool>>());
		}

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to1_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::false_type) const{
			{//Scoped
				std::lock_guard<std::mutex> guard(lock_);
				value_manager_.add_(receiver, msg);
				wparam = value_manager_.add_(wparam);
			}
			
			return send_to2_(item_placeholders.allocated_wparam_only.get(), msg, wparam, lparam, std::bool_constant<is_basic_value<wparam_type>::value>());
		}

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to1b_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::true_type) const{
			return send_to2_(receiver, msg, (wparam ? TRUE : FALSE), lparam, std::bool_constant<is_basic_value<lparam_type>::value>());
		}

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to1b_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::false_type) const{
			return send_to2_(receiver, msg, wparam, lparam, std::bool_constant<is_basic_value<lparam_type>::value>());
		}

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to2_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::true_type) const{
			return send_to2b_(receiver, msg, wparam, lparam, std::bool_constant<std::is_same_v<wparam_type, bool>>());
		}

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to2_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::false_type) const{
			if (receiver != item_placeholders.allocated_wparam_only.get()){
				std::lock_guard<std::mutex> guard(lock_);
				value_manager_.add_(receiver, msg);
				lparam = value_manager_.add_(lparam);
				receiver = item_placeholders.allocated_lparam_only.get();
			}
			else{
				std::lock_guard<std::mutex> guard(lock_);
				lparam = value_manager_.add_(lparam);
				receiver = item_placeholders.allocated_both.get();
			}

			return do_send_(receiver, msg, wparam, lparam);
		}

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to2b_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::true_type) const{
			return do_send_(receiver, msg, wparam, (lparam ? TRUE : FALSE));
		}

		template <typename return_type, typename wparam_type, typename lparam_type>
		return_type send_to2b_(const item *receiver, unsigned int msg, const wparam_type &wparam, const lparam_type &lparam, std::false_type) const{
			return do_send_(receiver, msg, wparam, lparam);
		}

		virtual LRESULT do_send_(const item *receiver, unsigned int msg, WPARAM wparam, LPARAM lparam) const;

		value_manager::managed_info_type pop_managed_();

		std::shared_ptr<value> pop_value_(unsigned __int64 key);

		queue_type queue_;
		std::thread::id id_;
		DWORD local_id_ = 0;

		state_type state_ = state_type::nil;
		value_manager value_manager_;

		bool is_main_;
		std::list<item *> list_;
		mutable std::mutex lock_;
	};
}
