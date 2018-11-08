#pragma once

#include "event_handler.h"

namespace winp::message{
	class dispatcher;
}

namespace winp::event{
	class dispatcher{
	protected:
		friend class message::dispatcher;

		virtual void dispatch_(object &e);
	};

	class create_destroy_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class draw_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};

	class mouse_dispatcher : public dispatcher{
	protected:
		virtual void dispatch_(object &e) override;
	};
}
