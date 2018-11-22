#pragma once

#include "menu_item_component.h"

namespace winp::menu{
	class separator : public item_component{
	public:
		separator();

		explicit separator(thread::object &thread);

		explicit separator(ui::tree &parent);

		virtual ~separator();

	protected:
		friend class object;
		friend class wrapper;

		template <class> friend class generic_collection_base;

		virtual bool validate_child_insert_(const ui::object &child, std::size_t index) const override;

		virtual bool validate_child_remove_(const ui::object &child) const override;

		virtual UINT get_persistent_states_() const;

		virtual UINT get_types_() const override;
	};
}
