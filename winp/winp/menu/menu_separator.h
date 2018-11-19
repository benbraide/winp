#pragma once

#include "../ui/ui_surface.h"

#include "menu_component.h"

namespace winp::menu{
	class object;
	class wrapper;

	template <class base_type>
	class generic_collection;

	class separator : public ui::surface, public component{
	public:
		separator();

		explicit separator(thread::object &thread);

		explicit separator(ui::tree &parent);

		virtual ~separator();

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const override;

	protected:
		friend class object;
		friend class wrapper;

		template <class> friend class generic_collection;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual void index_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual bool validate_child_insert_(const ui::object &child, std::size_t index) const override;

		virtual bool validate_child_remove_(const ui::object &child) const override;

		virtual std::size_t get_count_() const override;

		virtual std::size_t get_absolute_index_() const;

		virtual bool remove_from_parent_(ui::tree &parent);

		virtual bool update_(const MENUITEMINFOW &info);

		virtual bool update_states_();

		virtual bool update_types_();

		virtual void destruct_();

		bool is_created_ = false;
	};
}
