#pragma once

#include <functional>
#include <unordered_map>

#include "../thread/thread_object.h"
#include "../utility/random_number.h"

#include "event_object.h"

namespace winp::event{
	template <class owner_type, class object_type, class return_type>
	class manager{
	public:
		using m_object_type = object_type;
		using m_return_type = return_type;

		using m_callback_type = std::function<m_return_type(m_object_type &)>;
		using m_no_arg_callback_type = std::function<m_return_type()>;

		using m_map_type = std::unordered_map<unsigned __int64, m_callback_type>;

		unsigned __int64 operator +=(const m_callback_type &handler){
			unsigned __int64 key = rand_;
			if (thread_ == nullptr){
				handlers_[key] = handler;
				return key;
			}
			
			return thread_->queue->add([this, handler, key]{
				handlers_[key] = handler;
				return key;
			}, thread::queue::send_priority).get();
		}

		unsigned __int64 operator +=(const m_no_arg_callback_type &handler){
			return operator +=([handler](m_object_type &){
				return handler();
			});
		}

		bool operator -=(unsigned __int64 id){
			if (thread_ == nullptr){
				auto it = handlers_->find(id);
				if (it == handlers_->end())
					return false;

				handlers_->erase(it);
				return true;
			}

			return thread_->queue->add([&]{
				auto it = handlers_->find(id);
				if (it == handlers_->end())
					return false;

				handlers_->erase(it);
				return true;
			}, thread::queue::send_priority).get();
		}

	private:
		friend owner_type;

		template <typename dummy_type = return_type>
		std::enable_if_t<std::is_void_v<dummy_type>> fire_(m_object_type &e) const{
			for (auto &item : handlers_){
				item.second(e);
				if ((e.state_ & object::state_type::propagation_stopped) != 0u)
					break;
			}
		}

		template <typename dummy_type = return_type>
		std::enable_if_t<!std::is_void_v<dummy_type>> fire_(m_object_type &e, const dummy_type &default_value = dummy_type()) const{
			auto value = default_value;
			for (auto &item : handlers_){
				value = item.second(e);
				if ((e.state_ & object::state_type::propagation_stopped) != 0u)
					break;
			}

			return value;
		}

		m_map_type handlers_;
		utility::random_integral_number rand_;
		thread::object *thread_ = nullptr;
	};
}
