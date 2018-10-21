#pragma once

#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <future>
#include <memory>
#include <variant>
#include <functional>
#include <unordered_map>

namespace winp::thread{
	class object;

	class queue{
	public:
		using callback_type = std::function<void()>;

		struct list_item_info{
			unsigned __int64 id;
			callback_type callback;
		};

		using list_type = std::map<int, std::list<list_item_info>, std::greater<>>;

		struct added_info_type{
			std::list<list_item_info> *list = nullptr;
			list_item_info *it;
		};

		template <class target_type>
		class future{
		public:
			using m_target_type = target_type;
			using m_callback_type = std::function<m_target_type()>;
			using m_future_type = std::future<m_target_type>;
			using m_promise_type = std::promise<target_type>;

			struct m_immediate_info_type{
				std::list<list_item_info> *list = nullptr;
				list_item_info *it;
				m_callback_type task;
				queue *owner = nullptr;
			};

			using m_value_type = std::variant<m_future_type, m_immediate_info_type, char>;

			explicit future(std::shared_ptr<m_promise_type> promise)
				: value_(promise->get_future()), promise_(promise){}

			future(std::list<list_item_info> &list, list_item_info &it, m_callback_type task, queue &owner)
				: value_(m_immediate_info_type{ &list, &it, task, &owner }){}

			template <typename unused_type = target_type>
			std::enable_if_t<!std::is_void_v<unused_type>, m_target_type> get(){
				if (std::holds_alternative<char>(value_))
					return m_target_type();
				return get_();
			}

			template <typename unused_type = target_type>
			std::enable_if_t<std::is_void_v<unused_type>, m_target_type> get(){
				if (!std::holds_alternative<char>(value_))
					get_();
			}

		private:
			friend class queue;

			future()
				: value_('\0'){}

			m_target_type get_(){
				if (std::holds_alternative<m_immediate_info_type>(value_)){
					auto &info = std::get<m_immediate_info_type>(value_);
					{//Scoped
						std::lock_guard<std::mutex> guard(info.owner->lock_);
						auto it = std::find_if(info.list->begin(), info.list->end(), [&info](const list_item_info &item_info){
							return (&item_info == info.it);
						});

						if (it != info.list->end())
							info.list->erase(it);
					}
					
					return info.task();
				}
				
				return std::get<m_future_type>(value_).get();
			}

			m_value_type value_;
			std::shared_ptr<m_promise_type> promise_;
		};

		template <typename function_type>
		auto add(const function_type &task, int priority = 0, unsigned __int64 id = 0u){
			using return_type = decltype(task());
			if (is_inside_thread_context_()){
				auto info = add_([this, task, id]{
					if (!is_black_listed_(id))
						task();
				}, priority, id);

				if (info.list == nullptr || info.it == nullptr)
					return future<return_type>();

				return future<return_type>(*info.list, *info.it, task, *this);
			}

			auto promise = std::make_shared<std::promise<return_type>>();
			auto info = add_([this, task, id, promise]{
				if (is_black_listed_(id))
					set_null_promise_value_<return_type>(*promise, std::bool_constant<std::is_void_v<return_type>>());
				else
					set_promise_value_(*promise, task, std::bool_constant<std::is_void_v<return_type>>());
			}, priority, id);

			if (info.list == nullptr || info.it == nullptr)
				return future<return_type>();

			return future<return_type>(promise);
		}

		void post(const callback_type &task, int priority = 0, unsigned __int64 id = 0u);

		object *get_thread() const;

		static const int send_priority = 99999;

	protected:
		friend class object;
		template <class> friend class future;

		explicit queue(object &thread);

		void add_to_black_list_(unsigned __int64 id);

		bool is_black_listed_(unsigned __int64 id) const;

		added_info_type add_(const callback_type &task, int priority, unsigned __int64 id);

		void pop_all_send_priorities_(std::list<callback_type> &list);

		callback_type pop_send_priority_();

		callback_type pop_();

		bool is_inside_thread_context_() const;

		template <typename return_type, typename promise_type>
		void set_null_promise_value_(promise_type &promise, std::true_type){
			promise.set_value();
		}

		template <typename return_type, typename promise_type>
		void set_null_promise_value_(promise_type &promise, std::false_type){
			promise.set_value(return_type());
		}

		template <typename promise_type, typename function_type>
		void set_promise_value_(promise_type &promise, const function_type &task, std::true_type){
			promise.set_value();
		}

		template <typename promise_type, typename function_type>
		void set_promise_value_(promise_type &promise, const function_type &task, std::false_type){
			promise.set_value(task());
		}

		object &thread_;
		std::mutex lock_;

		list_type list_;
		std::unordered_map<unsigned __int64, char> black_list_;
	};
}
