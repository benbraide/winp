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

		explicit tree(tree &parent);

		virtual ~tree();

		prop::list<std::list<object *>, tree, prop::proxy_value> children;

		static const unsigned __int64 child_change_id			= (1ui64 << (last_object_change_id_bit + 0x00000001ui64));

		static const unsigned __int64 last_tree_change_id_bit	= (last_object_change_id_bit + 0x00000003ui64);

	protected:
		friend class object;

		friend class message::dispatcher;
		friend class thread::surface_manager;

		void init_();

		virtual void do_request_(void *buf, const std::type_info &id) override;

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

		virtual void fire_ancestor_change_event_(tree *value, std::size_t index) const override;

		virtual bool fire_child_change_event_(bool is_changing, object &child, std::size_t index) const;

		virtual void fire_child_sibling_change_event_(object &child, std::size_t previous_index, std::size_t current_index) const;

		std::list<object *> children_;
	};
}
