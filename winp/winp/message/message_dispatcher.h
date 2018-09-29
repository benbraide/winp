#pragma once

#include "../ui/window_surface.h"

#include "message_object.h"

namespace winp::thread{
	class windows_manager;
}

namespace winp::message{
	class dispatcher{
	protected:
		friend class thread::windows_manager;

		virtual bool dispatch_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const;

		virtual bool fire_event_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const;
	};

	class create_destroy_dispatcher : public dispatcher{
	protected:
		virtual bool dispatch_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const override;

		virtual bool fire_event_(ui::window_surface &target, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT &result) const override;
	};
}
