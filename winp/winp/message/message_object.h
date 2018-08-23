#pragma once

#include "../property/scalar_property.h"
#include "../property/error_property.h"

#include "../thread/thread_value.h"

namespace winp::thread{
	class item;
	class object;
}

namespace winp::message{
	class object{
	public:
		using m_item_type = thread::item;
		using m_default_callback_type = std::function<void(object &)>;
		using m_thread_value_type = thread::value;

		struct state_type{
			static constexpr unsigned int nil						= (0 << 0x0000);
			static constexpr unsigned int default_done				= (1 << 0x0000);
			static constexpr unsigned int default_prevented			= (1 << 0x0001);
			static constexpr unsigned int propagation_stopped		= (1 << 0x0002);
			static constexpr unsigned int event_propagation_stopped	= (1 << 0x0003);
		};

		object(thread::object &thread_owner, m_item_type *target);

		virtual ~object();

		prop::scalar<m_item_type *, object, prop::proxy_value> owner;
		prop::scalar<m_item_type *, object, prop::proxy_value> target;

		prop::scalar<bool, object, prop::proxy_value> do_default;
		prop::scalar<bool, object, prop::proxy_value> prevent_default;

		prop::scalar<bool, object, prop::proxy_value> stop_propagation;
		prop::scalar<bool, object, prop::proxy_value> stop_event_propagation;

	protected:
		friend class thread::item;
		friend class thread::object;

		virtual bool bubble_();

		virtual void do_default_();

		void pop_managed_(m_item_type *&receiver, unsigned int &msg);

		std::shared_ptr<m_thread_value_type> pop_value_(unsigned __int64 key);

		thread::object *thread_owner_;
		m_item_type *owner_;
		m_item_type *target_;
		unsigned int state_;
	};

	class basic : public object{
	public:
		struct info_type{
			unsigned int code;
			WPARAM wparam;
			LPARAM lparam;
		};

		explicit basic(thread::object &thread_owner, m_item_type *target, const info_type &info, m_default_callback_type default_callback = nullptr);

		virtual ~basic();

		template <typename value_type>
		void set_result(const value_type &value){
			set_result_(value, std::bool_constant<std::is_same_v<value_type, bool>>());
		}

		prop::scalar<info_type *, object, prop::proxy_value> target;
		prop::scalar<LRESULT, object, prop::proxy_value> result;

	protected:
		friend class thread::item;
		friend class thread::object;

		virtual void do_default_() override;

		template <typename value_type>
		void set_result_(const value_type &value, std::true_type){
			result_ = (value ? TRUE : FALSE);
		}

		template <typename value_type>
		void set_result_(const value_type &value, std::false_type){
			result_ = (LRESULT)value;
		}

		void update_managed_();

		void update_wparam_();

		void update_lparam_();

		info_type info_;
		std::shared_ptr<m_thread_value_type> wparam_;
		std::shared_ptr<m_thread_value_type> lparam_;

		LRESULT result_;
		m_default_callback_type default_callback_;
	};
}
