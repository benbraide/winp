#pragma once

#include "event_handler.h"

namespace winp::message{
	class dispatcher;
}

namespace winp::event{
	class unhandled_handler;
	class draw_handler;

	class dispatcher{
	protected:
		friend class message::dispatcher;

		virtual void dispatch_(object &e);

		static void set_result_of_(event::object &e, LRESULT value, bool always_set);

		static LRESULT get_result_of_(event::object &e);

		static bool result_set_of_(event::object &e);

		static bool default_prevented_of_(event::object &e);
	};

	class tree_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class create_destroy_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class draw_dispatcher : public dispatcher{
	protected:
		friend class unhandled_handler;
		friend class draw_handler;

		virtual void dispatch_(object &e) override;

		static void erase_background_(draw &e);
	};

	class cursor_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class mouse_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class focus_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class key_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};
}
