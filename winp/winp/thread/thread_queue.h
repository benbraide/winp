#pragma once

#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <memory>
#include <future>
#include <variant>
#include <functional>
#include <unordered_map>

namespace winp::thread{
	class object;

	class queue{
	public:
		using callback_type = std::function<void()>;

		using list_type = std::list<callback_type>;
		using map_type = std::map<int, list_type, std::greater<>>;

		template <typename function_type>
		auto execute(const function_type &task, int priority = 0, unsigned __int64 id = 0u){
			using return_type = decltype(task());
			if (is_inside_thread_context_())
				return call_<return_type>(task, id, std::bool_constant<std::is_void_v<return_type>>());

			auto promise = std::promise<return_type>();
			add_([&]{
				if (is_black_listed_(id))
					set_null_promise_value_<return_type>(promise, std::bool_constant<std::is_void_v<return_type>>());
				else
					set_promise_value_(promise, task, std::bool_constant<std::is_void_v<return_type>>());
			}, priority);

			return promise.get_future().get();
		}

		void post(const callback_type &task, int priority = 0, unsigned __int64 id = 0u);

		object &get_thread();

		static const int send_priority = 99999;

	protected:
		friend class object;

		explicit queue(object &thread);

		void add_to_black_list_(unsigned __int64 id);

		void remove_from_black_list_(unsigned __int64 id);

		bool is_black_listed_(unsigned __int64 id) const;

		void add_(const callback_type &task, int priority);

		void pop_all_send_priorities_(std::list<callback_type> &list);

		callback_type pop_send_priority_();

		callback_type pop_();

		bool is_inside_thread_context_() const;

		template <typename return_type, typename function_type>
		return_type call_(const function_type &task, unsigned __int64 id, std::false_type){
			return (is_black_listed_(id) ? return_type() : task());
		}

		template <typename return_type, typename function_type>
		return_type call_(const function_type &task, unsigned __int64 id, std::true_type){
			if (!is_black_listed_(id))
				task();
		}

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
			task();
			promise.set_value();
		}

		template <typename promise_type, typename function_type>
		void set_promise_value_(promise_type &promise, const function_type &task, std::false_type){
			promise.set_value(task());
		}

		object &thread_;
		std::mutex lock_;

		map_type map_;
		std::unordered_map<unsigned __int64, char> black_list_;
	};
}
