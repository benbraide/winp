#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::window{
	class frame : public ui::window_surface{
	public:
		frame();

		explicit frame(thread::object &thread);

		explicit frame(ui::window_surface &parent);

		virtual ~frame();

		virtual void set_caption(const std::wstring &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual std::wstring get_caption(const std::function<void(const std::wstring &)> &callback = nullptr) const;
	};
}
