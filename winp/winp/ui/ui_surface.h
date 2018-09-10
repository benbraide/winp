#pragma once

#include "../property/pair_property.h"
#include "../property/quad_property.h"

#include "ui_tree.h"

namespace winp::ui{
	class surface : public tree{
	public:
		using m_size_type = utility::size<int>;
		using m_point_type = utility::point<int>;
		using m_rgba_type = utility::rgba<float>;

		explicit surface(thread::object &thread);

		explicit surface(tree &parent);

		virtual ~surface();

		//virtual bool transition(const prop::owned_base<surface> &prop);

		prop::scalar<bool, surface, prop::proxy_value> visible;
		prop::scalar<bool, surface, prop::proxy_value> transparent;

		prop::size<surface, int> size;
		prop::point<surface, int> position;
		prop::point<surface, int> absolute_position;

		prop::rgba<surface, float> color;

		static const unsigned int state_nil					= (0 << 0x0000);
		static const unsigned int state_visible				= (1 << 0x0000);
		static const unsigned int state_transparent			= (1 << 0x0001);

	protected:
		void init_();

		virtual surface *get_surface_parent_() const;

		virtual void set_state_(unsigned int value);

		virtual void remove_state_(unsigned int value);

		virtual bool has_state_(unsigned int value) const;

		virtual void set_size_(const m_size_type &value);

		virtual m_size_type get_size_() const;

		virtual void set_position_(const m_point_type &value);

		virtual m_point_type get_position_() const;

		virtual void set_absolute_position_(const m_point_type &value);

		virtual m_point_type get_absolute_position_() const;

		virtual void set_color_(const m_rgba_type &value);

		virtual m_rgba_type get_color_() const;

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const;

		unsigned int state_ = state_nil;
		m_size_type size_{};
		m_point_type position_{};
		m_rgba_type color_;
	};
}
