#pragma once

#include "menu_item_component.h"

namespace winp::menu{
	class item : public item_component{
	public:
		item();

		explicit item(bool);

		explicit item(thread::object &thread);

		item(thread::object &thread, bool);

		explicit item(ui::tree &parent);

		item(ui::tree &parent, bool);

		virtual ~item();

		virtual ui::surface *get_popup(const std::function<void(ui::surface *)> &callback = nullptr) const;

		virtual bool set_label(const std::wstring &value, const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual const std::wstring *get_label(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual bool set_shortcut(const std::wstring &value, const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual const std::wstring *get_shortcut(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual bool make_default(const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual bool is_default(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool set_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual HBITMAP get_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		virtual bool select(const std::function<void(item_component &, bool)> &callback = nullptr);

		event::manager<item, event::object> init_event{ *this };
		event::manager<item, event::object> select_event{ *this };
		event::manager<item, event::object> check_event{ *this };
		event::manager<item, event::object> uncheck_event{ *this };

	protected:
		friend class menu::object;
		friend class menu::wrapper;
		friend class menu::group;

		template <class> friend class menu::generic_collection_base;
		friend class thread::surface_manager;

		virtual bool handle_child_insert_event_(event::tree &e) override;

		virtual bool handle_child_remove_event_(event::tree &e) override;

		virtual void handle_child_inserted_event_(event::tree &e) override;

		virtual void handle_child_removed_event_(event::tree &e) override;

		virtual ui::surface *get_popup_() const override;

		virtual const std::wstring *get_label_() const override;

		virtual const std::wstring *get_shortcut_() const override;

		virtual HBITMAP get_bitmap_() const override;

		virtual bool set_label_(const std::wstring &value);

		virtual bool set_shortcut_(const std::wstring &value);

		virtual bool set_bitmap_(HBITMAP value);

		virtual bool select_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states);

		virtual bool select_(UINT msg, ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states);

		virtual bool update_popup_();

		virtual bool update_label_();

		virtual bool update_check_marks_();

		ui::surface *popup_ = nullptr;

		std::wstring label_;
		std::wstring shortcut_;

		HBITMAP bitmap_ = nullptr;
		HBITMAP checked_bitmap_ = nullptr;
		HBITMAP unchecked_bitmap_ = nullptr;
	};
}
