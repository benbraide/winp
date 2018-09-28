#pragma once

#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <future>
#include <memory>
#include <variant>
#include <functional>

#include "../property/scalar_property.h"

namespace winp::thread{
	class object;

	class queue{
	public:
		using callback_type = std::function<void()>;
		using list_type = std::map<int, std::list<callback_type>, std::greater<>>;

		struct added_info_type{
			std::list<callback_type> *list = nullptr;
			callback_type *it;
		};

		template <class target_type>
		class future{
		public:
			using m_target_type = target_type;
			using m_callback_type = std::function<m_target_type()>;
			using m_future_type = std::future<m_target_type>;

			struct m_immediate_info_type{
				std::list<callback_type> *list = nullptr;
				callback_type *it;
				m_callback_type task;
				queue *owner = nullptr;
			};

			using m_value_type = std::variant<m_future_type, m_immediate_info_type>;

			explicit future(std::promise<target_type> &promise)
				: value_(promise.get_future()){}

			future(std::list<callback_type> &list, callback_type &it, m_callback_type task, queue &owner)
				: value_(m_immediate_info_type{ &list, &it, task, &owner }){}

			m_target_type get(){
				if (std::holds_alternative<m_immediate_info_type>(value_)){
					auto &info = std::get<m_immediate_info_type>(value_);
					{//Scoped
						std::lock_guard<std::mutex> guard(info.owner->lock_);
						auto it = std::find_if(info.list->begin(), info.list->end(), [&info](const callback_type &task){
							return (&task == info.it);
						});

						if (it != info.list->end())
							info.list->erase(it);
					}
					
					return info.task();
				}
				
				return std::get<m_future_type>(value_).get();
			}

		private:
			m_value_type value_;
		};

		template <typename function_type>
		auto add(const function_type &task, int priority = 0){
			using return_type = decltype(task());
			if (is_inside_thread_context_()){
				auto info = add_([task]{
					task();
				}, priority);

				return future<return_type>(*info.list, *info.it, task, *this);
			}

			auto promise = std::make_shared<std::promise<return_type>>();
			add_([=]{
				set_promise_value_(*promise, task, std::bool_constant<std::is_void_v<return_type>>());
			}, priority);

			return future<return_type>(*promise);
		}

		void post(const callback_type &task, int priority = 0);

		prop::scalar<const object *, queue, prop::proxy_value> owner;

		static const int send_priority = 99999;

	protected:
		friend class object;
		template <class> friend class future;

		explicit queue(object &owner);

		added_info_type add_(const callback_type &task, int priority);

		void pop_all_send_priorities_(std::list<callback_type> &list);

		callback_type pop_send_priority_();

		callback_type pop_();

		void wait_for_tasks_();

		bool is_empty_() const;

		bool is_inside_thread_context_() const;

		template <typename promise_type, typename function_type>
		void set_promise_value_(promise_type &promise, const function_type &task, std::true_type){
			task();
			promise.set_value();
		}

		template <typename promise_type, typename function_type>
		void set_promise_value_(promise_type &promise, const function_type &task, std::false_type){
			promise.set_value(task());
		}

		object *owner_;
		list_type list_;
		std::mutex lock_;
		std::condition_variable cv_;
	};
}
