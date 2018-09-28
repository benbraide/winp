#pragma once

#include "ui_visible_surface.h"

namespace winp::thread{
	class windows_manager;
}

namespace winp::message{
	class create_destroy_dispatcher;
}

namespace winp::ui{
	class window_surface : public visible_surface{
	public:
		using m_event_type = event::manager<window_surface, event::object, void>;

		using create_event_type = event_manager<m_event_type, event_id_type::create>;
		using destroy_event_type = event_manager<m_event_type, event_id_type::destroy>;

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

	protected:
		friend class thread::windows_manager;
		friend class message::create_destroy_dispatcher;

		void init_();

		virtual void destroy_() override;

		virtual void do_request_(void *buf, const std::type_info &id) override;

		virtual void do_apply_(const void *value, const std::type_info &id) override;

		virtual HWND get_handle_() const override;

		virtual void set_size_(const m_size_type &value) override;

		virtual m_size_type get_client_position_offset_() const override;

		virtual void set_position_(const m_point_type &value) override;

		virtual m_point_type convert_position_from_absolute_value_(const m_point_type &value) const override;

		virtual m_point_type convert_position_to_absolute_value_(const m_point_type &value) const override;

		virtual window_surface *get_window_surface_parent_() const;

		virtual void set_handle_(HWND value);

		virtual void set_message_entry_(LONG_PTR value);

		virtual void add_to_toplevel_();

		virtual void create_();

		virtual void set_styles_(DWORD value, bool is_extended);

		virtual DWORD get_styles_(bool is_extended) const;

		virtual DWORD get_persistent_styles_() const;

		virtual DWORD get_persistent_extended_styles_() const;

		virtual DWORD get_filtered_styles_() const;

		virtual DWORD get_filtered_extended_styles_() const;

		virtual HINSTANCE get_instance_() const;

		virtual WNDPROC get_default_message_entry_() const;

		virtual const wchar_t *get_class_name_() const;

		virtual const wchar_t *get_window_text_() const;

		virtual bool resolve_parent_(HWND &handle, m_point_type &position, DWORD &styles) const;

		virtual void fire_event_(m_event_type &ev, event::object &e) const;

		DWORD styles_;
		DWORD extended_styles_;

		m_event_type create_event_;
		m_event_type destroy_event_;
	};
}