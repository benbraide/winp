#pragma once

#include <list>

#include "ui_object.h"

namespace winp::menu{
	class check_item;
}

namespace winp::ui{
	class tree : public object{
	public:
		tree();

		explicit tree(thread::object &thread);

		virtual ~tree();

		virtual std::size_t add_child(object &child, const std::function<void(thread::item &, std::size_t)> &callback = nullptr);

		virtual std::size_t add_child(object &child, std::size_t index, const std::function<void(thread::item &, std::size_t)> &callback = nullptr);

		virtual bool remove_child(object &child, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool remove_child_at(std::size_t index, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual std::size_t find_child(const object &child, const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual object *get_child_at(std::size_t index, const std::function<void(object *)> &callback = nullptr) const;

		virtual void traverse_children(const std::function<void(object *)> &callback, bool post = true) const;

		event::manager<tree, event::tree> child_index_change_event{ *this };
		event::manager<tree, event::tree> child_insert_event{ *this };
		event::manager<tree, event::tree> child_remove_event{ *this };

		event::manager<tree, event::tree> child_index_changed_event{ *this };
		event::manager<tree, event::tree> child_inserted_event{ *this };
		event::manager<tree, event::tree> child_removed_event{ *this };

	protected:
		friend class object;

		friend class menu::check_item;
		friend class message::dispatcher;
		friend class thread::surface_manager;

		virtual std::size_t add_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual std::size_t insert_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual bool erase_child_(object &child);

		virtual bool remove_child_(object &child);

		virtual bool erase_child_at_(std::size_t index);

		virtual bool remove_child_at_(std::size_t index);

		virtual std::size_t change_child_index_(object &child, std::size_t index);

		virtual std::size_t find_child_(const object &child) const;

		virtual object *get_child_at_(std::size_t index) const;

		std::list<object *> children_;
	};
}
