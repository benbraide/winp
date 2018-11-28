#pragma once

#include "menu_link.h"
#include "menu_separator.h"

namespace winp::menu{
	class group : public ui::surface, public tree, public event::tree_handler{
	public:
		group();

		explicit group(thread::object &thread);

		explicit group(menu::object &parent);

		virtual ~group();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

		virtual menu::item_component *find_component(UINT id, const std::function<void(menu::item_component *)> &callback = nullptr) const override;

		virtual menu::item_component *get_component_at_absolute_index(std::size_t index, const std::function<void(menu::item_component *)> &callback = nullptr) const override;

	protected:
		friend class menu::item_component;

		virtual void destruct_() override;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual HANDLE get_handle_() const override;

		virtual UINT get_types_(std::size_t index) const override;

		virtual UINT get_states_(std::size_t index) const override;

		virtual std::size_t get_absolute_index_of_(const menu::component &child, bool skip_this) const override;

		virtual menu::item_component *find_component_(UINT id, item_component *exclude) const override;

		virtual menu::item_component *get_component_at_absolute_index_(std::size_t index) const override;

		virtual std::size_t get_count_() const override;

		virtual bool handle_parent_change_event_(event::tree &e) override;

		virtual bool handle_child_insert_event_(event::tree &e) override;

		virtual void handle_parent_changed_event_(event::tree &e) override;

		virtual void handle_index_changed_event_(event::tree &e) override;

		virtual std::size_t get_absolute_index_() const;

		virtual void update_children_types_();
	};
}
