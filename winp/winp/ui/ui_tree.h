#pragma once

#include <list>

#include "ui_object.h"

namespace winp::menu{
	class check_item;
}

namespace winp::ui{
	class tree : public object{
	public:
		struct child_change_info{
			object *child;
			tree *previous_parent_;
			std::size_t previous_index_;
		};

		tree();

		explicit tree(thread::object &thread);

		virtual ~tree();

		virtual std::size_t add_child(object &child, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual std::size_t add_child(object &child, std::size_t index, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual bool remove_child(object &child, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool remove_child_at(std::size_t index, const std::function<void(object &, bool)> &callback = nullptr);

		virtual std::size_t find_child(const object &child, const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual object *get_child_at(std::size_t index, const std::function<void(object *)> &callback = nullptr) const;

		virtual void traverse_children(const std::function<void(object *)> &callback, bool post = true) const;

		event::manager<tree, event::tree> child_index_change_event{ *this };
		event::manager<tree, event::tree> child_insert_event{ *this };
		event::manager<tree, event::tree> child_remove_event{ *this };

	protected:
		friend class object;

		friend class menu::check_item;
		friend class message::dispatcher;
		friend class thread::surface_manager;

		virtual bool validate_child_insert_(const object &child, std::size_t index) const;

		virtual std::size_t add_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual std::size_t insert_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual void child_inserted_(object &child, tree *previous_parent, std::size_t previous_index);

		virtual bool validate_child_remove_(const object &child) const;

		virtual bool erase_child_(object &child);

		virtual bool remove_child_(object &child);

		virtual bool erase_child_at_(std::size_t index);

		virtual bool remove_child_at_(std::size_t index);

		virtual void child_removed_(object &child, std::size_t previous_index);

		virtual bool validate_child_index_change_(const object &child, std::size_t index) const;

		virtual std::size_t change_child_index_(object &child, std::size_t index);

		virtual void child_index_changed_(object &child, tree *previous_parent, std::size_t previous_index);

		virtual std::size_t find_child_(const object &child) const;

		virtual object *get_child_at_(std::size_t index) const;

		std::list<object *> children_;
	};
}
