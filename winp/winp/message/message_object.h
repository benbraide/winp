#pragma once

#include "../property/scalar_property.h"
#include "../property/error_property.h"

#include "../thread/thread_value.h"

namespace winp::thread{
	class item;
	class object;
}

namespace winp::ui{
	class object;
}

namespace winp::message{
	class object{
	public:
		using m_default_callback_type = std::function<void(object &)>;
		using m_thread_value_type = thread::value;

		struct state_type{
			static constexpr unsigned int nil						= (0 << 0x0000);
			static constexpr unsigned int default_done				= (1 << 0x0000);
			static constexpr unsigned int default_prevented			= (1 << 0x0001);
			static constexpr unsigned int propagation_stopped		= (1 << 0x0002);
			static constexpr unsigned int event_propagation_stopped	= (1 << 0x0003);
			static constexpr unsigned int result_set				= (1 << 0x0004);
		};

		object(thread::object &thread, ui::object *target);

		virtual ~object();

		prop::scalar<thread::object *, object, prop::proxy_value> thread;
		prop::scalar<ui::object *, object, prop::proxy_value> owner;
		prop::scalar<ui::object *, object, prop::proxy_value> target;

		prop::scalar<bool, object, prop::proxy_value> do_default;
		prop::scalar<bool, object, prop::proxy_value> prevent_default;

		prop::scalar<bool, object, prop::proxy_value> stop_propagation;
		prop::scalar<bool, object, prop::proxy_value> stop_event_propagation;

	protected:
		friend class ui::object;
		friend class thread::object;

		virtual bool bubble_();

		virtual void do_default_();

		thread::object *thread_;
		ui::object *owner_;
		ui::object *target_;
		unsigned int state_;
	};

	class basic : public object{
	public:
		struct info_type{
			unsigned int code;
			WPARAM wparam;
			LPARAM lparam;
		};

		explicit basic(thread::object &thread, ui::object *target, const info_type &info, m_default_callback_type default_callback = nullptr);

		virtual ~basic();

		prop::scalar<info_type *, basic, prop::proxy_value> info;
		prop::scalar<LRESULT, basic, prop::proxy_value> result;

	protected:
		friend class thread::item;
		friend class thread::object;

		virtual void do_default_() override;

		info_type info_;
		LRESULT result_;
		m_default_callback_type default_callback_;
	};
}
