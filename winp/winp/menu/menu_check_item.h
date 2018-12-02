#pragma once

#include "menu_item.h"

namespace winp::menu{
	class check_item : public item{
	public:
		using object_ptr_type = std::shared_ptr<object>;

		check_item();

		explicit check_item(thread::object &thread);

		explicit check_item(ui::tree &parent);

		virtual ~check_item();

		virtual bool is_radio(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool check(const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual bool uncheck(const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual bool toggle_check(const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual bool is_checked(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool set_checked_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual HBITMAP get_checked_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		virtual bool set_unchecked_bitmap(HBITMAP value, const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual HBITMAP get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

	protected:
		friend class menu::object;
		friend class menu::wrapper;
		friend class menu::group;

		template <class> friend class menu::generic_collection_base;
		friend class thread::surface_manager;

		friend class event::draw_item_dispatcher;

		virtual bool handle_child_insert_event_(event::tree &e) override;

		virtual bool handle_child_remove_event_(event::tree &e) override;

		virtual HBITMAP get_unchecked_bitmap_() const override;

		virtual HBITMAP get_checked_bitmap_() const override;

		virtual bool select_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states) override;

		virtual bool is_radio_() const;

		virtual bool check_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states);

		virtual bool uncheck_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states, bool force);

		virtual bool toggle_check_(ui::surface *target, const MSG *info, bool prevent_default, unsigned int &states);

		virtual bool uncheck_siblings_(ui::tree &parent, ui::surface *target, bool force);

		virtual bool set_checked_bitmap_(HBITMAP value);

		virtual bool set_unchecked_bitmap_(HBITMAP value);
	};
}
