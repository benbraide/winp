#pragma once

#include <cstdlib>

namespace winp::ui{
	class object;
	class tree;

	class parent_change_attribute{
	public:
		virtual ~parent_change_attribute();

	protected:
		friend class object;

		virtual bool handle_parent_change_validation_(tree *value, std::size_t index) const;

		virtual bool handle_index_change_validation_(std::size_t value) const;

		virtual void handle_parent_changing_();

		virtual void handle_parent_changed_(tree *previous_parent, std::size_t previous_index);

		virtual void handle_index_changing_();

		virtual void handle_index_changed_(tree *previous_parent, std::size_t previous_index);
	};
}
