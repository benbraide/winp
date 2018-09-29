#pragma once

#include "ui_surface.h"

namespace winp::ui{
	class visible_surface : public surface{
	public:
		using m_rgba_type = utility::rgba<float>;

		using m_event_type = event::manager<visible_surface, event::object, void>;
		using show_event_type = event_manager<m_event_type, event_id_type::show>;
		using hide_event_type = event_manager<m_event_type, event_id_type::hide>;

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

		/*static const unsigned int state_nil					= (0 << 0x0000);
		static const unsigned int state_visible				= (1 << 0x0000);
		static const unsigned int state_transparent			= (1 << 0x0001);*/

	protected:
		void init_();

		virtual void do_request_(void *buf, const std::type_info &id) override;

		virtual void do_apply_(const void *value, const std::type_info &id) override;

		virtual visible_surface *get_visible_surface_parent_() const;

		/*virtual void toggle_state_(unsigned int value, bool set);

		virtual void set_state_(unsigned int value);

		virtual void remove_state_(unsigned int value);

		virtual bool has_state_(unsigned int value) const;*/

		virtual void set_visible_state_(bool state);

		virtual bool get_visible_state_() const;

		virtual void set_transaprent_state_(bool state);

		virtual bool get_transaprent_state_() const;

		virtual void set_background_color_(const m_rgba_type &value);

		virtual m_rgba_type get_background_color_() const;

		//unsigned int state_ = state_nil;
		m_rgba_type background_color_;
		m_event_type show_event_;
		m_event_type hide_event_;
	};
}
