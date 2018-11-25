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

		virtual void count_changed_(std::size_t previous_count){
			owner_.event_handlers_count_changed_(*this, previous_count, count_());
		}

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
			if (manager_base::get_thread_().is_thread_context()){
				auto result = bind_(handler);
				if (callback != nullptr)
					callback(*this, result);
				return result;
			}

			if (callback != nullptr){
				manager_base::get_thread_().queue.post([=]{ callback(*this, bind_(handler)); }, thread::queue::send_priority, manager_base::get_owner_id_());
				return 0u;
			}

			return manager_base::get_thread_().queue.execute([=]{ return bind_(handler); }, thread::queue::send_priority, manager_base::get_owner_id_());
		}

		unsigned __int64 bind(const m_no_arg_callback_type &handler, const std::function<void(manager_base &, unsigned __int64)> &callback = nullptr){
			return bind([handler](m_object_type &){
				return handler();
			}, callback);
		}

		bool unbind(unsigned __int64 id, const std::function<void(manager_base &, bool)> &callback = nullptr){
			if (manager_base::get_thread_().is_thread_context()){
				auto result = unbind_(id);
				if (callback != nullptr)
					callback(*this, result);
				return result;
			}

			if (callback != nullptr){
				manager_base::get_thread_().queue.post([=]{ callback(*this, unbind_(id)); }, thread::queue::send_priority, manager_base::get_owner_id_());
				return false;
			}

			return manager_base::get_thread_().queue.execute([=]{ return unbind_(id); }, thread::queue::send_priority, manager_base::get_owner_id_());
		}

	protected:
		friend owner_type;
		friend group_type;

		explicit manager(owner_type &owner)
			: manager_base(owner){}

		virtual std::size_t count_() const override{
			return handlers_.size();
		}

		virtual void fire_generic_(object &e) const override{
			fire_(dynamic_cast<m_object_type &>(e));
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
			auto previous_count = handlers_.size();

			handlers_[id] = handler;
			count_changed_(previous_count);

			return id;
		}

		bool unbind_(unsigned __int64 id){
			if (handlers_.empty())
				return false;

			auto it = handlers_->find(id);
			if (it == handlers_->end())
				return false;

			auto previous_count = handlers_.size();
			handlers_->erase(it);
			count_changed_(previous_count);

			return true;
		}

		m_map_type handlers_;
		utility::random_integral_number rand_;
	};
}
