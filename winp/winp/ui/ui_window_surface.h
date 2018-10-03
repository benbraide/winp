#pragma once

#include "ui_visible_surface.h"

namespace winp::message{
	class create_destroy_dispatcher;
}

namespace winp::ui{
	class window_surface : public visible_surface{
	public:
		enum class window_state{
			restored,
			maximized,
			minimized,
		};

		explicit window_surface(thread::object &thread);

		explicit window_surface(tree &parent);

		virtual ~window_surface();

		prop::flag_list<DWORD, window_surface, prop::proxy_value> styles;
		prop::flag_list<DWORD, window_surface, prop::proxy_value> extended_styles;

		prop::scalar<bool, window_surface, prop::proxy_value> created;
		prop::scalar<bool, window_surface, prop::proxy_value> maximized;
		prop::scalar<bool, window_surface, prop::proxy_value> minimized;

		event::manager<window_surface, event::object> create_event;
		event::manager<window_surface, event::object> destroy_event;

	protected:
		friend class message::dispatcher;
		friend class thread::windows_manager;

		void init_();

		virtual void destroy_() override;

		virtual void do_request_(void *buf, const std::type_info &id) override;

		virtual void do_apply_(const void *value, const std::type_info &id) override;

		virtual void set_size_(const m_size_type &value) override;

		virtual m_size_type get_size_() const override;

		virtual m_size_type get_client_position_offset_() const override;

		virtual void set_position_(const m_point_type &value) override;

		virtual m_point_type get_position_() const override;

		virtual m_point_type get_absolute_position_() const override;

		virtual m_rect_type get_dimension_() const override;

		virtual m_rect_type get_absolute_dimension_() const override;

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const override;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const override;

		virtual m_rect_type convert_dimension_from_absolute_value_(const m_rect_type &value) const override;

		virtual m_rect_type convert_dimension_to_absolute_value_(const m_rect_type &value) const override;

		virtual void set_visible_state_(bool state) override;

		virtual bool get_visible_state_() const override;

		virtual void set_transaprent_state_(bool state) override;

		virtual bool get_transaprent_state_() const override;

		virtual window_surface *get_window_surface_parent_() const;

		virtual void set_message_entry_(LONG_PTR value);

		virtual void add_to_toplevel_();

		virtual void create_();

		virtual void set_maximized_state_(bool state);

		virtual bool get_maximized_state_() const;

		virtual void set_minimized_state_(bool state);

		virtual bool get_minimized_state_() const;

		virtual void set_styles_(DWORD value, bool is_extended);

		virtual void add_styles_(DWORD value, bool is_extended);

		virtual void remove_styles_(DWORD value, bool is_extended);

		virtual bool has_styles_(DWORD value, bool is_extended, bool has_all = false) const;

		virtual DWORD get_styles_(bool is_extended) const;

		virtual DWORD get_persistent_styles_() const;

		virtual DWORD get_persistent_extended_styles_() const;

		virtual DWORD get_filtered_styles_() const;

		virtual DWORD get_filtered_extended_styles_() const;

		virtual HINSTANCE get_instance_() const;

		virtual WNDPROC get_default_message_entry_() const;

		virtual const wchar_t *get_class_name_() const;

		virtual const wchar_t *get_window_text_() const;

		virtual HWND get_first_window_ancestor_handle_() const;

		DWORD styles_;
		DWORD extended_styles_;
	};
}
