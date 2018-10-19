#pragma once

#include <list>

#include "ui_object.h"

namespace winp::ui{
	class tree : public object{
	public:
		struct child_change_info{
			object *child;
			std::size_t index;
		};

		explicit tree(thread::object &thread);

		virtual ~tree();

		virtual void add_child(object &child, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual void add_child(object &child, std::size_t index, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual void remove_child(object &child, const std::function<void(object &, bool)> &callback = nullptr);

		virtual void remove_child_at(std::size_t index, const std::function<void(object &, bool)> &callback = nullptr);

		virtual std::size_t find_child(const object &child, const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual object *get_child_at(std::size_t index, const std::function<void(object *)> &callback = nullptr) const;

		virtual void traverse_children(const std::function<void(object *)> &callback, bool post = true) const;

	protected:
		friend class object;

		friend class message::dispatcher;
		friend class thread::surface_manager;

		virtual bool validate_child_insert_(const object &child, std::size_t index) const;

		virtual std::size_t add_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual std::size_t insert_child_(object &child, std::size_t index = static_cast<std::size_t>(-1));

		virtual void child_inserted_(object &child, std::size_t index);

		virtual bool validate_child_remove_(const object &child) const;

		virtual bool erase_child_(object &child);

		virtual bool remove_child_(object &child);

		virtual bool erase_child_at_(std::size_t index);

		virtual bool remove_child_at_(std::size_t index);

		virtual void child_removed_(object &child, std::size_t index);

		virtual bool validate_child_index_change_(const object &child, std::size_t index) const;

		virtual std::size_t change_child_index_(object &child, std::size_t index);

		virtual void child_index_changed_(object &child, std::size_t previous_index, std::size_t index);

		virtual std::size_t find_child_(const object &child) const;

		virtual object *get_child_at_(std::size_t index) const;

		std::list<object *> children_;
	};
}
