#pragma once

#include "../ui/ui_window_surface.h"
#include "../event/event_handler.h"

namespace winp::control{
	class object : public ui::window_surface, public event::unhandled_handler{
	public:
		object();

		explicit object(thread::object &thread);

		virtual ~object();

		virtual bool set_font(HFONT value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual HFONT get_font(const std::function<void(HFONT)> &callback = nullptr) const;

		virtual bool set_text(const std::wstring &value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual std::wstring get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		static m_size_type compute_size(HWND handle, HDC device, HFONT font, const std::wstring &text);

	protected:
		friend class event::draw_item_dispatcher;
		friend class menu::object;

		virtual const wchar_t *get_theme_name_() const override;

		virtual void add_to_toplevel_(bool update = false) override;

		virtual void post_create_() override;

		virtual bool set_padding_(const m_rect_type &value) override;

		virtual HINSTANCE get_instance_() const override;

		virtual const wchar_t *get_window_text_() const override;

		virtual DWORD get_filtered_styles_() const override;

		virtual DWORD get_filtered_extended_styles_() const override;

		virtual void handle_unhandled_event_(event::object &e) override;

		virtual bool set_font_(HFONT value);

		virtual HFONT get_font_() const;

		virtual void font_changed_(HFONT old_font);

		virtual bool set_text_(const std::wstring &value);

		virtual const std::wstring &get_text_() const;

		virtual void padding_changed_();

		virtual void update_size_();

		virtual m_size_type get_computed_size_() const;

		virtual m_size_type compute_size_() const;

		virtual m_size_type compute_additional_size_(const m_size_type &size) const;

		virtual bool is_uniform_padding_() const;

		std::wstring text_;
		HFONT font_ = nullptr;
	};
}
