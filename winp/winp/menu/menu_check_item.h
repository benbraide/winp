#pragma once

#include "menu_item.h"

namespace winp::menu{
	class check_item : public item{
	public:
		using object_ptr_type = std::shared_ptr<object>;

		check_item();

		explicit check_item(thread::object &thread);

		explicit check_item(ui::tree &parent);

		check_item(ui::tree &parent, bool);

		virtual ~check_item();

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

		virtual HBITMAP get_unchecked_bitmap_() const override;

		virtual HBITMAP get_checked_bitmap_() const override;

		virtual bool select_() override;

		virtual bool check_();

		virtual bool uncheck_();

		virtual bool toggle_check_();

		virtual bool set_checked_bitmap_(HBITMAP value);

		virtual bool set_unchecked_bitmap_(HBITMAP value);

	};
}
