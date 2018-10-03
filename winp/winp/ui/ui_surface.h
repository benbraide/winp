#pragma once

#include "../property/map_property.h"
#include "../property/pair_property.h"
#include "../property/quad_property.h"

#include "ui_tree.h"

namespace winp::ui{
	class surface;
	class window_surface;

	class surface_hit_test{
	public:
		using m_size_type = utility::size<int>;
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

		virtual utility::hit_target hit_test_(const m_point_type &pt, const m_point_type &pos, const m_size_type &size) const;

		surface *surface_ = nullptr;
	};

	class surface : public tree{
	public:
		using m_size_type = utility::size<int>;
		using m_point_type = utility::point<int>;
		using m_rect_type = utility::rect<int>;

		explicit surface(thread::object &thread);

		explicit surface(tree &parent);

		virtual ~surface();

		prop::size<surface, int> size;
		prop::size<surface, int> client_position_offset;

		prop::point<surface, int> position;
		prop::point<surface, int> absolute_position;

		prop::rect<surface, int> dimension;
		prop::rect<surface, int> absolute_dimension;

		prop::map<m_point_type, m_point_type, surface> position_from_absolute;
		prop::map<m_point_type, m_point_type, surface> position_to_absolute;

		event::manager<surface, event::object> size_event;
		event::manager<surface, event::object> move_event;

	protected:
		friend class surface_hit_test;
		friend class window_surface;
		friend class thread::windows_manager;

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

		virtual m_rect_type get_dimension_() const;

		virtual m_rect_type get_absolute_dimension_() const;

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const;

		virtual m_rect_type convert_dimension_from_absolute_value_(const m_rect_type &value) const;

		virtual m_rect_type convert_dimension_to_absolute_value_(const m_rect_type &value) const;

		template <typename target_type>
		m_point_type get_offset_from_ancestor_of_(const m_point_type &ref) const{
			auto parent = get_surface_parent_();
			if (parent == nullptr || dynamic_cast<target_type *>(parent) != nullptr)
				return ref;

			auto parent_position = parent->get_position_();
			return parent->get_offset_from_ancestor_of_<target_type>(m_point_type{ (parent_position.x + ref.x), (parent_position.y + ref.y) });
		}

		m_size_type size_{};
		m_point_type position_{};
	};
}
