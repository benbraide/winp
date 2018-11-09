#pragma once

#include "ui_io_surface.h"

namespace winp::message{
	class create_destroy_dispatcher;
}

namespace winp::ui{
	class window_surface : public io_surface{
	public:
		enum class window_state{
			restored,
			maximized,
			minimized,
		};

		window_surface();

		explicit window_surface(thread::object &thread);

		virtual ~window_surface();

		virtual void maximize(const std::function<void(object &, bool)> &callback = nullptr);

		virtual void restore_maximized(const std::function<void(object &, bool)> &callback = nullptr);

		virtual void toggle_maximized(const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool is_maximized(const std::function<void(bool)> &callback = nullptr) const;

		virtual void minimize(const std::function<void(object &, bool)> &callback = nullptr);

		virtual void restore_minimized(const std::function<void(object &, bool)> &callback = nullptr);

		virtual void toggle_minimized(const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool is_minimized(const std::function<void(bool)> &callback = nullptr) const;

		virtual void set_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback = nullptr);

		virtual void add_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback = nullptr);

		virtual void remove_styles(DWORD value, bool is_extended, const std::function<void(object &, bool)> &callback = nullptr);

		virtual DWORD get_styles(bool is_extended, const std::function<void(DWORD)> &callback = nullptr) const;

		virtual bool has_styles(DWORD value, bool is_extended, bool has_all, const std::function<void(bool)> &callback = nullptr) const;

		event::manager<window_surface, event::object> create_event;
		event::manager<window_surface, event::object> destroy_event;

	protected:
		friend class message::dispatcher;
		friend class thread::surface_manager;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual WNDPROC get_default_message_entry_() const override;

		virtual bool set_size_(const m_size_type &value) override;

		virtual m_size_type get_size_() const override;

		virtual m_size_type get_client_position_offset_() const override;

		virtual bool set_position_(const m_point_type &value) override;

		virtual m_point_type get_position_() const override;

		virtual m_point_type get_absolute_position_() const override;

		virtual m_rect_type get_dimension_() const override;

		virtual m_rect_type get_absolute_dimension_() const override;

		virtual m_rect_type get_client_dimension_() const override;

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const override;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const override;

		virtual m_rect_type convert_dimension_from_absolute_value_(const m_rect_type &value) const override;

		virtual m_rect_type convert_dimension_to_absolute_value_(const m_rect_type &value) const override;

		virtual void redraw_(const m_rect_type &region) override;

		virtual bool set_visibility_(bool is_visible) override;

		virtual bool is_visible_() const override;

		virtual bool set_transparency_(bool is_transparent) override;

		virtual bool is_transparent_() const override;

		virtual utility::hit_target hit_test_(const m_point_type &pt, bool is_absolute) const override;

		virtual utility::hit_target hit_test_(const m_rect_type &rect, bool is_absolute) const override;

		virtual void destruct_();

		virtual bool pre_create_();

		virtual void post_create_();

		virtual window_surface *get_window_surface_parent_() const;

		virtual bool maximize_();

		virtual bool restore_maximized_();

		virtual bool toggle_maximized_();

		virtual bool is_maximized_() const;

		virtual bool minimize_();

		virtual bool restore_minimized_();

		virtual bool toggle_minimized_();

		virtual bool is_minimized_() const;

		virtual bool set_styles_(DWORD value, bool is_extended);

		virtual bool add_styles_(DWORD value, bool is_extended);

		virtual bool remove_styles_(DWORD value, bool is_extended);

		virtual bool has_styles_(DWORD value, bool is_extended, bool has_all) const;

		virtual DWORD get_styles_(bool is_extended) const;

		virtual DWORD get_persistent_styles_() const;

		virtual DWORD get_persistent_extended_styles_() const;

		virtual DWORD get_filtered_styles_() const;

		virtual DWORD get_filtered_extended_styles_() const;

		virtual HINSTANCE get_instance_() const;

		virtual const wchar_t *get_class_name_() const;

		virtual const wchar_t *get_window_text_() const;

		virtual HWND get_first_window_ancestor_handle_() const;

		DWORD styles_;
		DWORD extended_styles_;
	};
}
