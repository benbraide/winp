#pragma once

#include "../utility/windows.h"
#include "../property/scalar_property.h"

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

		explicit object(ui::object *target);

		virtual ~object();

		prop::scalar<ui::object *, object, prop::proxy_value> owner;
		prop::scalar<ui::object *, object, prop::proxy_value> target;

	protected:
		friend class ui::object;
		friend class thread::object;

		ui::object *owner_;
		ui::object *target_;
	};

	class basic : public object{
	public:
		struct info_type{
			UINT code;
			WPARAM wparam;
			LPARAM lparam;
		};

		basic(ui::object *target, const info_type &info);

		virtual ~basic();

		prop::scalar<info_type *, basic, prop::proxy_value> info;
		prop::scalar<LRESULT, basic, prop::proxy_value> result;

	protected:
		friend class thread::item;
		friend class thread::object;

		info_type info_;
		LRESULT result_;
	};
}
