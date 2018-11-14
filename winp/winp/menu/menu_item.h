#pragma once

#include "../ui/ui_object.h"

namespace winp::menu{
	class item : public ui::object{
	public:
		item();

		explicit item(thread::object &thread);

		explicit item(ui::tree &parent);

		virtual ~item();

	protected:
		virtual bool validate_parent_change_(ui::tree *value, std::size_t index) const override;

		virtual void parent_changed_(ui::tree *previous_parent, std::size_t previous_index) override;

		virtual void index_changed_(ui::tree *previous_parent, std::size_t previous_index) override;
	};
}
