#pragma once

#include "ui_surface.h"

namespace winp::ui{
	class visible_surface : public surface{
	public:
		using m_rgba_type = utility::rgba<float>;//9GiFQvPq9KfmxgG

		enum class visibility{
			visible,
			hidden,
			transparent,
		};

		explicit visible_surface(thread::object &thread);

		explicit visible_surface(tree &parent);

		virtual ~visible_surface();

		prop::scalar<bool, visible_surface, prop::proxy_value> visible;
		prop::scalar<bool, visible_surface, prop::proxy_value> transparent;

		prop::rgba<visible_surface, float> background_color;

		event::manager<visible_surface, event::object> show_event;
		event::manager<visible_surface, event::object> hide_event;

		event::manager<visible_surface, event::object> background_erase_event;
		event::manager<visible_surface, event::object> paint_event;

		/*static const unsigned int state_nil					= (0 << 0x0000);
		static const unsigned int state_visible				= (1 << 0x0000);
		static const unsigned int state_transparent			= (1 << 0x0001);*/

	protected:
		friend class thread::surface_manager;

		void init_();

		virtual void do_request_(void *buf, const std::type_info &id) override;

		virtual void do_apply_(const void *value, const std::type_info &id) override;

		virtual visible_surface *get_visible_surface_parent_() const;

		/*virtual void toggle_state_(unsigned int value, bool set);

		virtual void set_state_(unsigned int value);

		virtual void remove_state_(unsigned int value);

		virtual bool has_state_(unsigned int value) const;*/

		virtual void redraw_();

		virtual void set_visible_state_(bool state);

		virtual bool get_visible_state_() const;

		virtual void set_transaprent_state_(bool state);

		virtual bool get_transaprent_state_() const;

		virtual void set_background_color_(const D2D1::ColorF &value);

		virtual void set_background_color_(const m_rgba_type &value);

		virtual D2D1::ColorF get_background_color_() const;

		virtual m_rgba_type get_converted_background_color_() const;

		//unsigned int state_ = state_nil;
		D2D1::ColorF background_color_;
	};
}
