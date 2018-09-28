#pragma once

#include "../property/map_property.h"
#include "../property/pair_property.h"
#include "../property/quad_property.h"

#include "ui_tree.h"

namespace winp::ui{
	class surface;

	class surface_hit_test{
	public:
		using m_point_type = utility::point<int>;
		using m_rect_type = utility::rect<int>;

		surface_hit_test();

		surface_hit_test(const surface_hit_test &copy);

		surface_hit_test &operator =(const surface_hit_test &copy);

		prop::variant<surface_hit_test, prop::immediate_value, m_point_type, m_rect_type> value;
		prop::scalar<bool, surface_hit_test, prop::immediate_value> is_absolute;
		prop::scalar<utility::hit_target, surface_hit_test, prop::proxy_value> target;

	private:
		friend class surface;

		void init_();

		virtual utility::hit_target hit_test_(const m_point_type &pt, const m_point_type &pos) const;

		surface *surface_ = nullptr;
	};

	class surface : public tree{
	public:
		using m_size_type = utility::size<int>;
		using m_point_type = utility::point<int>;
		using m_rect_type = utility::rect<int>;

		using m_event_type = event::manager<surface, event::object, void>;
		using size_event_type = event_manager<m_event_type, event_id_type::size>;
		using move_event_type = event_manager<m_event_type, event_id_type::move>;

		explicit surface(thread::object &thread);

		explicit surface(tree &parent);

		virtual ~surface();

		prop::size<surface, int> size;
		prop::size<surface, int> client_position_offset;

		prop::point<surface, int> position;
		prop::point<surface, int> absolute_position;

		prop::map<m_point_type, m_point_type, surface> position_from_absolute;
		prop::map<m_point_type, m_point_type, surface> position_to_absolute;

	protected:
		friend class surface_hit_test;

		void init_();

		virtual void do_request_(void *buf, const std::type_info &id) override;

		virtual surface *get_surface_parent_() const;

		virtual void set_size_(const m_size_type &value);

		virtual m_size_type get_size_() const;

		virtual m_size_type get_client_position_offset_() const;

		virtual void set_position_(const m_point_type &value);

		virtual m_point_type get_position_() const;

		virtual void set_absolute_position_(const m_point_type &value);

		virtual m_point_type get_absolute_position_() const;

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const;

		m_size_type size_{};
		m_point_type position_{};

		m_event_type size_event_;
		m_event_type move_event_;
	};
}