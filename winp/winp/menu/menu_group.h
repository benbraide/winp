#pragma once

#include "menu_link.h"
#include "menu_separator.h"

namespace winp::menu{
	class group : public ui::surface, public tree{
	public:
		group();

		explicit group(thread::object &thread);

		explicit group(menu::object &parent);

		virtual ~group();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

		virtual menu::item_component *find_component(UINT id, const std::function<void(menu::item_component *)> &callback = nullptr) const override;

		virtual menu::item_component *get_component_at_absolute_index(std::size_t index, const std::function<void(menu::item_component *)> &callback = nullptr) const override;

	protected:
		virtual void destruct_() override;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changing_() override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual void index_changing_() override;

		virtual void index_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual UINT get_types_(std::size_t index) const override;

		virtual UINT get_states_(std::size_t index) const override;

		virtual std::size_t get_absolute_index_of_(const menu::component &child) const override;

		virtual menu::item_component *find_component_(UINT id, item_component *exclude) const override;

		virtual menu::item_component *get_component_at_absolute_index_(std::size_t index) const override;

		virtual std::size_t get_count_() const override;

		virtual std::size_t get_absolute_index_() const;
	};
}
