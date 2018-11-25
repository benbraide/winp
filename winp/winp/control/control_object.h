#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::control{
	class object : public ui::window_surface{
	public:
		object();

		explicit object(thread::object &thread);

		virtual ~object();

		virtual bool set_font(HFONT value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual HFONT get_font(const std::function<void(HFONT)> &callback = nullptr) const;

		virtual bool set_text(const std::wstring &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual std::wstring get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		static m_size_type compute_size(HWND handle, HDC device, HFONT font, const std::wstring &text);

	protected:
		friend class menu::object;

		virtual void add_to_toplevel_(bool update = false) override;

		virtual void post_create_() override;

		virtual DWORD get_filtered_styles_() const override;

		virtual DWORD get_filtered_extended_styles_() const override;

		virtual bool set_font_(HFONT value);

		virtual HFONT get_font_() const;

		virtual bool set_text_(const std::wstring &value);

		virtual std::wstring get_text_() const;

		virtual void update_size_();

		virtual m_size_type compute_size_() const;

		virtual m_size_type compute_additional_size_() const;

		HFONT font_ = nullptr;
	};
}
