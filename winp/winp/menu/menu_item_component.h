#pragma once

#include "../ui/ui_surface.h"
#include "../event/event_handler.h"

#include "menu_tree.h"

namespace winp::event{
	class draw_item_dispatcher;
}

namespace winp::menu{
	class group;
	class object;
	class wrapper;

	template <class base_type>
	class generic_collection_base;

	class item_component : public ui::surface, public component, public event::tree_handler{
	public:
		item_component();

		explicit item_component(bool);

		explicit item_component(thread::object &thread);

		item_component(thread::object &thread, bool);

		explicit item_component(ui::tree &parent);

		item_component(ui::tree &parent, bool);

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

		virtual bool is_popup_item(const std::function<void(bool)> &callback = nullptr) const;

		event::manager<item_component, event::object> create_event{ *this };
		event::manager<item_component, event::object> destroy_event{ *this };

		event::manager<item_component, event::draw_item> draw_item_event{ *this };
		event::manager<item_component, event::measure_item> measure_item_event{ *this };

	protected:
		friend class event::measure_item;
		friend class event::draw_item_dispatcher;

		friend class menu::object;
		friend class menu::wrapper;
		friend class menu::group;

		template <class> friend class menu::generic_collection_base;
		friend class thread::surface_manager;

		virtual void event_handlers_count_changed_(event::manager_base &e, std::size_t previous_count, std::size_t current_count);

		virtual void destruct_() override;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual std::size_t get_count_() const override;

		virtual bool handle_parent_change_event_(event::tree &e) override;

		virtual void handle_parent_changed_event_(event::tree &e) override;

		virtual void handle_index_changed_event_(event::tree &e) override;

		virtual UINT get_local_id_() const;

		virtual std::size_t get_absolute_index_() const;

		virtual ui::surface *get_popup_() const;

		virtual const std::wstring *get_label_() const;

		virtual const std::wstring *get_shortcut_() const;

		virtual HFONT get_font_() const;

		virtual bool set_state_(UINT value);

		virtual bool remove_state_(UINT value);

		virtual UINT get_states_() const;

		virtual UINT get_persistent_states_() const;

		virtual UINT get_filtered_states_() const;

		virtual bool has_state_(UINT value) const;

		virtual bool has_states_(UINT value) const;

		virtual UINT get_types_() const;

		virtual bool has_type_(UINT value) const;

		virtual bool is_owner_drawn_() const;

		virtual bool is_popup_item_() const;

		virtual HBITMAP get_bitmap_() const;

		virtual HBITMAP get_checked_bitmap_() const;

		virtual HBITMAP get_unchecked_bitmap_() const;

		virtual bool update_(const MENUITEMINFOW &info);

		virtual bool update_states_();

		virtual bool update_types_();

		virtual void register_id_();

		virtual void generate_id_(std::size_t max_tries = 0xFFFFu);

		virtual bool id_is_unique_() const;

		UINT local_id_ = 0u;
		bool is_created_ = false;

		UINT states_ = 0u;
		UINT types_ = 0u;

		HFONT font_ = nullptr;
	};
}
