#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>

#include "../thread/thread_object.h"

namespace winp::utility{
	template <class precision_type = std::chrono::nanoseconds>
	class timer_interval{
	public:
		using m_precision_type = precision_type;

		using duration_type = std::chrono::duration<__int64, m_precision_type>;
		using callback_type = std::function<void(unsigned __int64, unsigned __int64)>;

		using condition_variable_type = std::condition_variable;
		using lock_type = std::mutex;

		using guard_type = std::lock_guard<lock_type>;
		using unique_guard_type = std::unique_lock<lock_type>;

		explicit timer_interval(thread::object &thread)
			: thread_(thread){}

		~timer_interval(){
			stop(true);
		}

		bool run(){
			if (!thread_.is_thread_context())
				return false;

			guard_type guard(*lock_);
			if (running_ || count_ == 0u || duration_ == m_precision_type(0))
				return true;

			auto context_id = context_id_;
			auto count = count_;

			auto duration = duration_;
			auto callback = callback_;

			running_ = true;
			std::thread([=]{
				unique_guard_type lock(*lock_);
				for (auto loop_count = 1ui64; running_ && (count == static_cast<unsigned __int64>(-1) || loop_count <= count); ++loop_count){
					cv_->wait_for(lock, duration, [this]{
						return !running_;
					});

					if (running_)
						thread_.queue.post(std::bind(callback, loop_count, count), 0, context_id);
				}

				if (notify_exit_){
					exit_wait_->notify_one();
					notify_exit_ = false;
				}
				
				running_ = false;
			}).detach();

			return true;
		}

		void stop(bool wait_for_exit){
			unique_guard_type lock(*lock_);
			if (!running_)
				return;

			running_ = false;
			cv_->notify_one();

			if (notify_exit_ = wait_for_exit){
				exit_wait_.wait(lock, [this]{
					return running_;
				});
			}
		}

		void set_count(unsigned __int64 value){
			if (thread_.is_thread_context())
				count_ = value;
		}

		void set_duration_(const m_precision_type &value){
			if (thread_.is_thread_context())
				duration_ = value;
		}

		void set_callback_(const callback_type &value, unsigned __int64 context_id = 0u){
			if (thread_.is_thread_context()){
				callback_ = value;
				context_id_ = context_id;
			}
		}

	private:
		template <class> friend class timer;

		thread::object &thread_;

		bool running_ = false;
		bool notify_exit_ = false;

		unsigned __int64 context_id_ = 0u;
		unsigned __int64 count_ = 0u;

		m_precision_type duration_ = m_precision_type(0);
		callback_type callback_;

		condition_variable_type cv_;
		condition_variable_type exit_wait_;
		lock_type lock_;
	};
}
