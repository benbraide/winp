#pragma once

#include "../ui/ui_surface.h"

#include "menu_tree.h"

namespace winp::menu{
	class group;
	class object;
	class wrapper;

	template <class base_type>
	class generic_collection_base;

	class item_component : public ui::surface, public component{
	public:
		item_component();

		explicit item_component(thread::object &thread);

		explicit item_component(ui::tree &parent);

		virtual ~item_component();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

		virtual UINT get_local_id(const std::function<void(UINT)> &callback = nullptr) const;

		virtual bool set_state(UINT value, const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual bool remove_state(UINT value, const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual UINT get_states(const std::function<void(UINT)> &callback = nullptr) const;

		virtual bool has_state(UINT value, const std::function<void(bool)> &callback = nullptr) const;

		virtual bool has_states(UINT value, const std::function<void(bool)> &callback = nullptr) const;

		virtual bool enable(const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual bool disable(const std::function<void(item_component &, bool)> &callback = nullptr);

		virtual bool is_disabled(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool is_owner_drawn(const std::function<void(bool)> &callback = nullptr) const;

	protected:
		friend class menu::object;
		friend class menu::wrapper;
		friend class menu::group;

		template <class> friend class menu::generic_collection_base;
		friend class thread::surface_manager;

		virtual void destruct_() override;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changing_() override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual void index_changing_() override;

		virtual void index_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual std::size_t get_count_() const override;

		virtual UINT get_local_id_() const;

		virtual std::size_t get_absolute_index_() const;

		virtual ui::surface *get_popup_() const;

		virtual const std::wstring *get_label_() const;

		virtual const std::wstring *get_shortcut_() const;

		virtual bool set_state_(UINT value);

		virtual bool remove_state_(UINT value);

		virtual UINT get_states_() const;

		virtual UINT get_persistent_states_() const;

		virtual UINT get_filtered_states_() const;

		virtual bool has_state_(UINT value) const;

		virtual bool has_states_(UINT value) const;

		virtual UINT get_types_() const;

		virtual bool has_type_(UINT value) const;

		virtual HBITMAP get_bitmap_() const;

		virtual HBITMAP get_checked_bitmap_() const;

		virtual HBITMAP get_unchecked_bitmap_() const;

		virtual bool update_(const MENUITEMINFOW &info);

		virtual bool update_states_();

		virtual bool update_types_();

		virtual void generate_id_(std::size_t max_tries = 0xFFFFu);

		UINT local_id_ = 0u;
		bool is_created_ = false;

		UINT states_ = 0u;
		UINT types_ = 0u;
	};
}
