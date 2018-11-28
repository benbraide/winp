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

		virtual bool handle_child_insert_event_(event::tree &e) override;

		virtual bool handle_child_remove_event_(event::tree &e) override;

		virtual UINT get_persistent_states_() const override;

		virtual UINT get_types_() const override;

		virtual void generate_id_(std::size_t max_tries = 0xFFFFu) override;
	};
}
