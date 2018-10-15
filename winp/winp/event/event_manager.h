#pragma once

#include <functional>
#include <unordered_map>

#include "../thread/thread_object.h"
#include "../utility/random_number.h"

#include "event_object.h"

namespace winp::event{
	class manager_base{
	protected:
		friend class ui::object;

		virtual std::size_t count_() const = 0;

		virtual void fire_generic_(object &e) const = 0;
	};

	template <class owner_type, class object_type>
	class manager : public manager_base{
	public:
		using m_object_type = object_type;

		using m_callback_type = std::function<void(m_object_type &)>;
		using m_no_arg_callback_type = std::function<void()>;

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

	protected:
		friend owner_type;

		virtual std::size_t count_() const override{
			return handlers_.size();
		}

		virtual void fire_generic_(object &e) const override{
			fire_(*dynamic_cast<m_object_type *>(&e));
		}

		virtual void fire_(m_object_type &e) const{
			for (auto &item : handlers_){
				item.second(e);
				if ((e.state_ & object::state_type::propagation_stopped) != 0u)
					break;
			}
		}

		m_map_type handlers_;
		utility::random_integral_number rand_;
		thread::object *thread_ = nullptr;
	};
}
