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

		explicit manager_base(thread::item &owner)
			: owner_(owner){}

		virtual ~manager_base() = default;

		virtual std::size_t count_() const = 0;

		virtual void fire_generic_(object &e) const = 0;

		virtual thread::object &get_thread_(){
			return owner_.thread_;
		}

		virtual unsigned __int64 get_owner_id_() const{
			return owner_.id_;
		}

		thread::item &owner_;
	};

	template <class owner_type, class object_type, class group_type>
	class manager : public manager_base{
	public:
		using m_object_type = object_type;

		using m_callback_type = std::function<void(m_object_type &)>;
		using m_no_arg_callback_type = std::function<void()>;

		using m_map_type = std::unordered_map<unsigned __int64, m_callback_type>;

		unsigned __int64 operator +=(const m_callback_type &handler){
			return bind(handler);
		}

		unsigned __int64 operator +=(const m_no_arg_callback_type &handler){
			return bind(handler);
		}

		bool operator -=(unsigned __int64 id){
			return unbind(id);
		}

		unsigned __int64 bind(const m_callback_type &handler, const std::function<void(manager_base &, unsigned __int64)> &callback = nullptr){
			if (callback != nullptr){
				manager_base::get_thread_().queue.post([=]{ callback(*this, bind_(handler)); }, thread::queue::send_priority, manager_base::get_owner_id_());
				return 0u;
			}

			return manager_base::get_thread_().queue.add([=]{ return bind_(handler); }, thread::queue::send_priority, manager_base::get_owner_id_()).get();
		}

		unsigned __int64 bind(const m_no_arg_callback_type &handler, const std::function<void(manager_base &, unsigned __int64)> &callback = nullptr){
			return bind([handler](m_object_type &){
				return handler();
			}, callback);
		}

		bool unbind(unsigned __int64 id, const std::function<void(manager_base &, bool)> &callback = nullptr){
			if (callback != nullptr){
				manager_base::get_thread_().queue.post([=]{ callback(*this, unbind_(id)); }, thread::queue::send_priority, manager_base::get_owner_id_());
				return false;
			}

			return manager_base::get_thread_().queue.add([=]{ return unbind_(id); }, thread::queue::send_priority, manager_base::get_owner_id_()).get();
		}

	protected:
		friend owner_type;
		friend group_type;

		explicit manager(owner_type &owner, const std::function<void(manager_base &, std::size_t)> &callback = nullptr)
			: manager_base(owner), callback_(callback){}

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

		unsigned __int64 bind_(const m_callback_type &handler){
			auto id = rand_(1ui64, std::numeric_limits<unsigned __int64>::max());
			handlers_[id] = handler;

			if (callback_ != nullptr)
				callback_(*this, handlers_.size());

			return id;
		}

		bool unbind_(unsigned __int64 id){
			if (handlers_.empty())
				return false;

			auto it = handlers_->find(id);
			if (it == handlers_->end())
				return false;

			handlers_->erase(it);
			if (callback_ != nullptr)
				callback_(*this, handlers_.size());

			return true;
		}

		m_map_type handlers_;
		std::function<void(manager_base &, std::size_t)> callback_;
		utility::random_integral_number rand_;
	};
}
