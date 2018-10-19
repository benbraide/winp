#pragma once

#include "../utility/windows.h"

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

		explicit object(ui::object &target);

		virtual ~object();

		virtual ui::object *get_target() const;

	protected:
		friend class ui::object;
		friend class thread::object;

		virtual bool is_thread_context_() const;

		ui::object *target_;
	};

	class basic : public object{
	public:
		struct info_type{
			UINT code;
			WPARAM wparam;
			LPARAM lparam;
		};

		basic(ui::object &target, const info_type &info);

		virtual ~basic();

		virtual info_type get_info() const;

		template <typename value_type>
		void set_result(value_type value){
			if (is_thread_context_())
				result_ = (LRESULT)value;
		}

		virtual void set_result(bool value);

		virtual LRESULT get_result() const;

	protected:
		friend class thread::item;
		friend class thread::object;

		info_type info_;
		LRESULT result_;
	};
}
