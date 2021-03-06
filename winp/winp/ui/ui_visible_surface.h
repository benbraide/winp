#pragma once

#include "ui_surface.h"

namespace winp::event{
	class draw_dispatcher;
	class unhandled_handler;
	class draw_handler;
}

namespace winp::ui{
	class visible_surface : public surface{
	public:
		struct m_colorf{
			float r;
			float g;
			float b;
			float a;
		};

		visible_surface();

		explicit visible_surface(thread::object &thread);

		virtual ~visible_surface();

		virtual bool show(const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool show(int how, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool hide(const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool set_visibility(bool is_visible, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool is_visible(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool set_transparency(bool is_transparent, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool is_transparent(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool set_background_color(const D2D1::ColorF &value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual D2D1::ColorF get_background_color(const std::function<void(const D2D1::ColorF &)> &callback = nullptr) const;

		static m_colorf convert_from_d2d1_colorf(const D2D1::ColorF &value);

		static D2D1::ColorF convert_to_d2d1_colorf(const m_colorf &value);

		static bool compare_colors(const D2D1::ColorF &first, const D2D1::ColorF &second);

		event::manager<visible_surface, event::object> show_event{ *this };
		event::manager<visible_surface, event::object> hide_event{ *this };

		event::manager<visible_surface, event::draw> background_erase_event{ *this };
		event::manager<visible_surface, event::draw> draw_event{ *this };

	protected:
		friend class non_window::child;

		friend class event::draw_dispatcher;
		friend class event::unhandled_handler;
		friend class event::draw_handler;
		friend class thread::surface_manager;

		void init_();

		virtual visible_surface *get_visible_surface_parent_() const;

		virtual void redraw_(const m_rect_type &region);

		virtual bool set_visibility_(bool is_visible);

		virtual bool is_visible_() const;

		virtual bool set_transparency_(bool is_transparent);

		virtual bool is_transparent_() const;

		virtual bool set_background_color_(const D2D1::ColorF &value);

		virtual const D2D1::ColorF &get_background_color_() const;

		D2D1::ColorF background_color_{ 0 };
	};
}
