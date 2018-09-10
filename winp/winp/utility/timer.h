#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>

#include "../thread/thread_object.h"
#include "../property/variant_property.h"

namespace winp::utility{
	template <class precision = std::chrono::nanoseconds>
	class timer;

	template <class precision = std::chrono::nanoseconds>
	class timer_interval{
	public:
		using m_precision = precision;

		using duration_type = std::chrono::duration<__int64, m_precision>;
		using callback_type = std::function<void(unsigned __int64, unsigned __int64, bool)>;

		using condition_variable_type = std::condition_variable;
		using lock_type = std::mutex;

		using guard_type = std::lock_guard<lock_type>;
		using unique_guard_type = std::unique_lock<lock_type>;

		timer_interval() = default;

		timer_interval(const timer_interval &copy)
			: thread_(copy.thread_){
			if (thread_ != nullptr)
				init_();
		}

		timer_interval &operator =(const timer_interval &copy){
			if ((thread_ = copy.thread_) != nullptr && lock_ == nullptr)
				init_();
			return *this;
		}

		prop::scalar<bool, timer_interval, prop::proxy_value> running;
		prop::scalar<unsigned __int64, timer_interval, prop::immediate_value> count;
		prop::scalar<m_precision, timer_interval, prop::immediate_value> duration;
		prop::scalar<callback_type, timer_interval, prop::immediate_value> callback;

	private:
		template <class> friend class timer;

		void init_(){
			auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
				if (&prop == &running)
					set_running_state_(*static_cast<const bool *>(value));
			};

			auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
				if (&prop == &running)
					*static_cast<bool *>(buf) = running_;
			};

			running.init_(nullptr, setter, getter);
			count.m_value_ = static_cast<unsigned __int64>(-1);

			cv_ = std::make_shared<condition_variable_type>();
			lock_ = std::make_shared<lock_type>();
		}

		void set_running_state_(bool state){
			if (state){//Run
				std::thread([this]{
					unique_guard_type lock(*lock_);

					auto count = this->count.m_value_;
					auto duration = this->duration.m_value_;
					auto callback = this->callback.m_value_;

					if (running_ || count == 0ui64 || duration == m_precision(0) || callback == nullptr)
						return;

					running_ = true;
					notified_ = false;

					for (auto loop_count = 1ui64; running_ && (count == static_cast<unsigned __int64>(-1) || loop_count <= count); ++loop_count){
						cv_->wait_for(lock, duration, [this]{
							return notified_;
						});

						notified_ = false;
						if (running_)
							thread_->queue->post(std::bind(callback, loop_count, count, true));
					}

					thread_->queue->post(std::bind(callback, 0ui64, count, false));
				}).detach();
			}
			else{//Stop
				guard_type guard(*lock_);
				if (!running_)
					return;

				running_ = false;
				notified_ = true;
				cv_->notify_one();
			}
		}

		thread::object *thread_ = nullptr;
		std::shared_ptr<condition_variable_type> cv_;
		std::shared_ptr<lock_type> lock_;

		bool running_ = false;
		bool notified_ = false;
	};

	template <class precision = std::chrono::nanoseconds>
	class timer_counter{
	public:
		using m_precision = precision;
		using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

		using duration_type = std::chrono::duration<__int64, m_precision>;
		using callback_type = std::function<void(unsigned __int64, unsigned __int64, bool)>;

		using condition_variable_type = std::condition_variable;
		using lock_type = std::mutex;

		using guard_type = std::lock_guard<lock_type>;
		using unique_guard_type = std::unique_lock<lock_type>;

		timer_counter() = default;

		timer_counter(const timer_counter &copy)
			: thread_(copy.thread_){
			if (thread_ != nullptr)
				init_();
		}

		timer_counter &operator =(const timer_counter &copy){
			if ((thread_ = copy.thread_) != nullptr && lock_ == nullptr)
				init_();
			return *this;
		}

		prop::scalar<bool, timer_counter, prop::proxy_value> running;
		prop::scalar<m_precision, timer_counter, prop::proxy_value> elapsed;

	private:
		template <class> friend class timer;

		void init_(){
			auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
				if (&prop == &running)
					set_running_state_(*static_cast<const bool *>(value));
			};

			auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
				if (&prop == &elapsed){
					unique_guard_type lock(*lock_);
					if (!running_)
						return;

					notified_ = true;
					cv_->notify_one();

					cv_wait_->wait(lock, [this]{
						return notified_wait_;
					});

					notified_wait_ = false;
					if (running_)
						*static_cast<m_precision *>(buf) = std::chrono::duration_cast<m_precision>(current_time_ - start_time_);
				}
				else if (&prop == &running)
					*static_cast<bool *>(buf) = running_;
			};

			running.init_(nullptr, setter, getter);
			elapsed.init_(nullptr, nullptr, getter);

			cv_ = std::make_shared<condition_variable_type>();
			cv_wait_ = std::make_shared<condition_variable_type>();
			lock_ = std::make_shared<lock_type>();
		}

		void set_running_state_(bool state){
			if (state){//Run
				std::thread([this]{
					unique_guard_type lock(*lock_);
					if (running_)
						return;

					running_ = true;
					notified_ = false;
					start_time_ = current_time_ = std::chrono::high_resolution_clock::now();

					while (running_){
						cv_->wait(lock, [this]{
							return notified_;
						});

						notified_ = false;
						start_time_ = current_time_;
						current_time_ = std::chrono::high_resolution_clock::now();

						notified_wait_ = true;
						cv_wait_->notify_one();
					}
				}).detach();
			}
			else{//Stop
				guard_type guard(*lock_);
				if (!running_)
					return;
				
				running_ = false;
				notified_ = true;
				cv_->notify_one();
			}
		}

		thread::object *thread_ = nullptr;
		std::shared_ptr<lock_type> lock_;

		std::shared_ptr<condition_variable_type> cv_;
		std::shared_ptr<condition_variable_type> cv_wait_;

		bool running_ = false;
		bool notified_ = false;
		bool notified_wait_ = false;

		time_point start_time_;
		time_point current_time_;
	};

	template <class precision>
	class timer{
	public:
		using m_precision = precision;

		using interval_type = timer_interval<m_precision>;
		using counter_type = timer_counter<m_precision>;

		using variant_type = prop::variant<timer, interval_type, counter_type>;

		explicit timer(thread::object &thread)
			: thread_(&thread){
			auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
				switch (context){
				case variant_type::m_type_list_type::template index_of<interval_type>:
					do_request_<interval_type>(buf);
					break;
				case variant_type::m_type_list_type::template index_of<counter_type>:
					do_request_<counter_type>(buf);
					break;
				default:
					break;
				}
			};

			request.init_(nullptr, nullptr, getter);
		}

		variant_type request;

	private:
		template <typename target_type>
		void do_request_(void *buf){
			static_cast<target_type *>(buf)->thread_ = thread_;
			static_cast<target_type *>(buf)->init_();
		}

		thread::object *thread_;
	};
}
