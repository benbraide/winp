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

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const;

		virtual m_rect_type convert_dimension_from_absolute_value_(const m_rect_type &value) const;

		virtual m_rect_type convert_dimension_to_absolute_value_(const m_rect_type &value) const;

		virtual UINT hit_test_(const m_point_type &pt, bool is_absolute) const;

		virtual utility::hit_target hit_test_(const m_rect_type &rect, bool is_absolute) const;

		virtual utility::hit_target hit_test_(const m_point_type &pt, const m_point_type &pos, const m_size_type &size) const;

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
