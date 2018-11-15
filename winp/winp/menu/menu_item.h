#pragma once

#include "../ui/ui_object.h"

#include "menu_tree.h"

namespace winp::menu{
	class item : public ui::object, public component{
	public:
		item();

		explicit item(thread::object &thread);

		explicit item(ui::tree &parent);

		virtual ~item();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

		virtual WORD get_local_id() const;

	protected:
		virtual bool destroy_() override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual void index_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual std::size_t get_count_() const override;

		virtual std::size_t get_absolute_index_() const;

		virtual bool remove_from_parent_(ui::tree &parent);

		virtual bool insert_into_parent_(ui::tree &parent);

		virtual bool update_(const MENUITEMINFOW &info);

		WORD local_id_;

		std::wstring label_;
		std::wstring shortcut_;

		UINT states_ = 0u;
		UINT types_ = 0u;
	};
}
