#pragma once

#include "../ui/ui_surface.h"

#include "menu_tree.h"

namespace winp::menu{
	class group;
	class object;
	class wrapper;

	template <class base_type>
	class generic_collection;

	class item : public ui::surface, public component{
	public:
		item();

		explicit item(thread::object &thread);

		explicit item(ui::tree &parent);

		item(ui::tree &parent, bool);

		virtual ~item();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

		virtual WORD get_local_id() const;

		virtual ui::surface *get_target(const std::function<void(ui::surface *)> &callback = nullptr) const;

		virtual void set_label(const std::wstring &value, const std::function<void(item &, bool)> &callback = nullptr);

		virtual std::wstring get_label(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual void set_shortcut(const std::wstring &value, const std::function<void(item &, bool)> &callback = nullptr);

		virtual std::wstring get_shortcut(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual void set_state(UINT value, const std::function<void(item &, bool)> &callback = nullptr);

		virtual void remove_state(UINT value, const std::function<void(item &, bool)> &callback = nullptr);

		virtual UINT get_states(const std::function<void(UINT)> &callback = nullptr) const;

		virtual bool has_state(UINT value, const std::function<void(bool)> &callback = nullptr) const;

		virtual bool has_states(UINT value, const std::function<void(bool)> &callback = nullptr) const;

		virtual void enable(const std::function<void(item &, bool)> &callback = nullptr);

		virtual void disable(const std::function<void(item &, bool)> &callback = nullptr);

		virtual bool is_disabled(const std::function<void(bool)> &callback = nullptr) const;

		virtual void make_default(const std::function<void(item &, bool)> &callback = nullptr);

		virtual bool is_default(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool is_owner_drawn(const std::function<void(bool)> &callback = nullptr) const;

		virtual void set_bitmap(HBITMAP value, const std::function<void(item &, bool)> &callback = nullptr);

		virtual HBITMAP get_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		virtual void set_checked_bitmap(HBITMAP value, const std::function<void(item &, bool)> &callback = nullptr);

		virtual HBITMAP get_checked_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		virtual void set_unchecked_bitmap(HBITMAP value, const std::function<void(item &, bool)> &callback = nullptr);

		virtual HBITMAP get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		event::manager<item, event::object> init_event{ *this };
		event::manager<item, event::object> select_event{ *this };
		event::manager<item, event::object> check_event{ *this };
		event::manager<item, event::object> uncheck_event{ *this };

	protected:
		friend class menu::object;
		friend class menu::wrapper;
		friend class menu::group;

		template <class> friend class generic_collection;
		friend class thread::surface_manager;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual void index_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual bool validate_child_insert_(const ui::object &child, std::size_t index) const override;

		virtual void child_inserted_(ui::object &child, tree *previous_parent, std::size_t previous_index) override;

		virtual bool validate_child_remove_(const ui::object &child) const override;

		virtual void child_removed_(ui::object &child, std::size_t previous_index) override;

		virtual std::size_t get_count_() const override;

		virtual std::size_t get_absolute_index_() const;

		virtual bool remove_from_parent_(ui::tree &parent);

		virtual ui::surface *get_target_() const;

		virtual bool set_label_(const std::wstring &value);

		virtual const std::wstring &get_label_() const;

		virtual bool set_shortcut_(const std::wstring &value);

		virtual const std::wstring &get_shortcut_() const;

		virtual bool set_state_(UINT value);

		virtual bool remove_state_(UINT value);

		virtual UINT get_states_() const;

		virtual UINT get_persistent_states_() const;

		virtual UINT get_filtered_states_() const;

		virtual bool has_state_(UINT value) const;

		virtual bool has_states_(UINT value) const;

		virtual bool set_bitmap_(HBITMAP value);

		virtual HBITMAP get_bitmap_() const;

		virtual bool set_checked_bitmap_(HBITMAP value);

		virtual HBITMAP get_checked_bitmap_() const;

		virtual bool set_unchecked_bitmap_(HBITMAP value);

		virtual HBITMAP get_unchecked_bitmap_() const;

		virtual bool has_type_(UINT value) const;

		virtual bool update_(const MENUITEMINFOW &info);

		virtual bool update_label_();

		virtual bool update_states_();

		virtual bool update_types_();

		virtual bool update_check_marks_();

		virtual void destruct_();

		virtual void generate_id_();

		WORD local_id_ = 0u;
		bool is_created_ = false;
		ui::surface *target_ = nullptr;

		std::wstring label_;
		std::wstring shortcut_;

		UINT states_ = 0u;
		UINT types_ = 0u;

		HBITMAP bitmap_ = nullptr;
		HBITMAP checked_bitmap_ = nullptr;
		HBITMAP unchecked_bitmap_ = nullptr;
	};
}
