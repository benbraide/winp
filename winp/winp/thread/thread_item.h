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

		virtual void post_task(const queue::callback_type &task, int priority = queue::send_priority) const;

		virtual void use_context(const queue::callback_type &task, int priority = queue::send_priority) const;

		template <typename function_type>
		auto execute_using_context(const function_type &task, int priority = queue::send_priority) const{
			return get_queue_().execute(task, priority, id_);
		}

		template <typename function_type, typename return_type>
		auto execute_with_default_value_using_context(const function_type &task, const return_type &default_value, int priority = queue::send_priority) const{
			return get_queue_().execute_with_default_value(task, default_value, priority, id_);
		}

		template <typename function_type>
		auto execute_or_post_task(const function_type &task, int priority = queue::send_priority) const{
			using return_type = decltype(task());
			if (!is_thread_context()){
				get_queue_().post(task, priority, id_);
				return return_type();
			}

			return get_queue_().execute(task, priority, id_);
		}

		virtual bool is_thread_context() const;

		virtual unsigned __int64 get_id() const;

	protected:
		friend class object;
		friend class app::object;
		friend class event::manager_base;

		virtual void destruct_();

		virtual void event_handlers_count_changed_(event::manager_base &e, std::size_t previous_count, std::size_t current_count);

		virtual queue &get_queue_() const;

		template <typename value_type>
		value_type pass_value_to_callback_(const std::function<void(item &, std::conditional_t<std::is_scalar_v<value_type>, value_type, const value_type &>)> &callback, const value_type &value){
			if (callback != nullptr)
				callback(*this, value);
			return value;
		}

		template <typename value_type>
		value_type pass_value_to_callback_(const std::function<void(std::conditional_t<std::is_scalar_v<value_type>, value_type, const value_type &>)> &callback, const value_type &value) const{
			if (callback != nullptr)
				callback(value);
			return value;
		}

		template <typename function_type>
		auto execute_or_post_(const function_type &task, bool post) const{
			using return_type = decltype(task());
			if (post && !is_thread_context()){
				post_task(task);
				return return_type();
			}

			return execute_using_context(task);
		}

		template <typename function_type, typename return_type>
		auto execute_or_post_(const function_type &task, bool post, const return_type &default_value) const{
			if (post && !is_thread_context()){
				post_task(task);
				return default_value;
			}

			return execute_with_default_value_using_context(task, default_value);
		}

		object &thread_;
		unsigned __int64 id_;
		bool destructed_ = false;
	};
}
