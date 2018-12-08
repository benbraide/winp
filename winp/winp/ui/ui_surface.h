#pragma once

#include "ui_tree.h"

namespace winp::non_window{
	class child;
}

namespace winp::ui{
	class window_surface;

	class surface : public tree{
	public:
		using m_size_type = SIZE;
		using m_point_type = POINT;
		using m_rect_type = RECT;

		surface();

		explicit surface(thread::object &thread);

		virtual ~surface();

		virtual bool set_size(const m_size_type &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool set_size(int width, int height, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool offset_size(const m_size_type &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool offset_size(int width, int height, const std::function<void(object &, bool)> &callback = nullptr);

		virtual m_size_type get_size(const std::function<void(const m_size_type &)> &callback = nullptr) const;

		virtual m_size_type get_client_size(const std::function<void(const m_size_type &)> &callback = nullptr) const;

		virtual m_point_type get_client_position_offset(const std::function<void(const m_point_type &)> &callback = nullptr) const;

		virtual bool set_position(const m_point_type &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool set_position(int x, int y, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool offset_position(const m_point_type &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool offset_position(int x, int y, const std::function<void(object &, bool)> &callback = nullptr);

		virtual m_point_type get_position(const std::function<void(const m_point_type &)> &callback = nullptr) const;

		virtual bool set_absolute_position(const m_point_type &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool set_absolute_position(int x, int y, const std::function<void(object &, bool)> &callback = nullptr);

		virtual m_point_type get_absolute_position(const std::function<void(const m_point_type &)> &callback = nullptr) const;

		virtual bool set_dimension(const m_point_type &offset, const m_size_type &size, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool set_dimension(int x, int y, int width, int height, const std::function<void(object &, bool)> &callback = nullptr);

		virtual m_rect_type get_dimension(const std::function<void(const m_rect_type &)> &callback = nullptr) const;

		virtual m_rect_type get_absolute_dimension(const std::function<void(const m_rect_type &)> &callback = nullptr) const;

		virtual bool set_padding(const m_rect_type &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool set_padding(int left, int top, int right, int bottom, const std::function<void(object &, bool)> &callback = nullptr);

		virtual m_rect_type get_padding(const std::function<void(const m_rect_type &)> &callback = nullptr) const;

		virtual m_point_type convert_position_from_absolute_value(const m_point_type &value, const std::function<void(const m_point_type &)> &callback = nullptr) const;

		virtual m_point_type convert_position_to_absolute_value(const m_point_type &value, const std::function<void(const m_point_type &)> &callback = nullptr) const;

		virtual m_rect_type convert_dimension_from_absolute_value(const m_rect_type &value, const std::function<void(const m_rect_type &)> &callback = nullptr) const;

		virtual m_rect_type convert_dimension_to_absolute_value(const m_rect_type &value, const std::function<void(const m_rect_type &)> &callback = nullptr) const;

		virtual UINT hit_test(const m_point_type &pt, bool is_absolute, const std::function<void(UINT)> &callback = nullptr) const;

		virtual utility::hit_target hit_test(const m_rect_type &rect, bool is_absolute, const std::function<void(utility::hit_target)> &callback = nullptr) const;

		virtual utility::hit_target hit_test(const m_point_type &pt, const m_point_type &pos, const m_size_type &size, const std::function<void(utility::hit_target)> &callback = nullptr) const;

		event::manager<surface, event::object> size_change_event{ *this };
		event::manager<surface, event::object> position_change_event{ *this };

		event::manager<surface, event::object> size_changed_event{ *this };
		event::manager<surface, event::object> position_changed_event{ *this };

	protected:
		friend class surface_hit_test;
		friend class window_surface;
		friend class non_window::child;

		friend class event::draw;
		friend class message::dispatcher;
		friend class thread::surface_manager;

		virtual void add_to_toplevel_(bool update = false) override;

		virtual surface *get_surface_parent_() const;

		virtual surface *get_root_surface_() const;

		virtual void set_message_entry_(LONG_PTR value);

		virtual bool set_size_(const m_size_type &value);

		virtual bool offset_size_(const m_size_type &value);

		virtual m_size_type get_size_() const;

		virtual m_size_type get_client_size_() const;

		virtual m_point_type get_client_position_offset_() const;

		virtual m_point_type compute_child_observable_offset_(const surface &child) const;

		template <typename target_type>
		m_point_type compute_child_offset_from_ancestor_of_(const surface &child) const{
			auto surface_parent = get_first_ancestor_of_<surface>();
			if (surface_parent == nullptr)
				return child.get_position_();

			auto observable_offset = compute_child_observable_offset_(child), offset_from_window = surface_parent->compute_child_offset_from_ancestor_of_<target_type>(*this);
			return m_point_type{ (observable_offset.x + offset_from_window.x), (observable_offset.y + offset_from_window.y) };
		}

		template <typename target_type>
		m_point_type compute_offset_from_ancestor_of_() const{
			auto surface_parent = get_first_ancestor_of_<surface>();
			return ((surface_parent == nullptr) ? get_position_() : surface_parent->compute_child_offset_from_ancestor_of_<target_type>(*this));
		}

		virtual bool set_position_(const m_point_type &value);

		virtual bool offset_position_(const m_point_type &value);

		virtual m_point_type get_position_() const;

		virtual m_point_type get_cursor_position_() const;

		virtual bool set_absolute_position_(const m_point_type &value);

		virtual m_point_type get_absolute_position_() const;

		virtual bool set_dimension_(const m_point_type &offset, const m_size_type &size);

		virtual m_rect_type get_dimension_() const;

		virtual m_rect_type get_absolute_dimension_() const;

		virtual m_rect_type get_client_dimension_() const;

		virtual bool set_padding_(const m_rect_type &value);

		virtual const m_rect_type &get_padding_() const;

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const;

		virtual m_rect_type convert_dimension_from_absolute_value_(const m_rect_type &value) const;

		virtual m_rect_type convert_dimension_to_absolute_value_(const m_rect_type &value) const;

		virtual UINT hit_test_(const m_point_type &pt, bool is_absolute) const;

		virtual utility::hit_target hit_test_(const m_rect_type &rect, bool is_absolute) const;

		virtual utility::hit_target hit_test_(const m_point_type &pt, const m_point_type &pos, const m_size_type &size) const;

		m_size_type size_{};
		m_point_type position_{};
		m_rect_type padding_{};
	};
}
