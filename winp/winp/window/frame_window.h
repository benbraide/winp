#pragma once

#include "../menu/menu_wrapper.h"

namespace winp::window{
	class frame : public ui::window_surface{
	public:
		frame();

		explicit frame(thread::object &thread);

		virtual ~frame();

		virtual void set_caption(const std::wstring &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual std::wstring get_caption(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		friend class thread::surface_manager;

		virtual DWORD get_persistent_styles_() const override;

		virtual DWORD get_filtered_styles_() const override;

		virtual const wchar_t *get_window_text_() const override;

		virtual bool set_caption_(const std::wstring &value);

		virtual const std::wstring &get_caption_() const;

		std::wstring caption_;
		menu::wrapper system_menu_;
	};
}
