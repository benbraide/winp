#pragma once

#include <list>

#include "ui_object.h"

namespace winp::ui{
	class tree : public object{
	public:
		tree();

		virtual ~tree();

		prop::list<std::list<object *>, tree, prop::proxy_value> children;

	protected:
		friend class object;

		virtual bool validate_child_insert_(object &child, std::size_t index) const;

		virtual std::size_t add_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual std::size_t insert_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual void child_inserted_(object &child, std::size_t index);

		virtual bool validate_child_remove_(object &child) const;

		virtual bool erase_child_(object &child);

		virtual bool remove_child_(object &child);

		virtual bool erase_child_at_(std::size_t index);

		virtual bool remove_child_at_(std::size_t index);

		virtual void child_removed_(object &child);

		virtual bool validate_child_index_change_(object &child, std::size_t index) const;

		virtual std::size_t change_child_index_(object &child, std::size_t index);

		virtual void child_index_changed_(object &child, std::size_t previous_index, std::size_t index);

		virtual std::size_t find_child_(const object &child) const;

		virtual object *get_child_at_(std::size_t index) const;

		std::list<object *> children_;
	};
}
