#pragma once

#include "../utility/dynamic_list.h"
#include "../property/list_property.h"

#include "../thread/thread_item.h"

namespace winp::ui{
	class tree;

	class object : public thread::item{
	public:
		explicit object(thread::object &thread);

		explicit object(tree &parent);

		virtual ~object();

		prop::scalar<tree *, object, prop::proxy_value> parent;
		prop::scalar<std::size_t, object, prop::proxy_value> index;

		prop::scalar<object *, object, prop::proxy_value> previous_sibling;
		prop::scalar<object *, object, prop::proxy_value> next_sibling;

		prop::list<utility::dynamic_list<tree, object>, object, prop::immediate_value> ancestors;
		prop::list<utility::dynamic_list<object, object>, object, prop::immediate_value> siblings;

	protected:
		friend class tree;

		void init_();

		virtual tree *get_parent_() const;

		virtual bool validate_parent_change_(tree *value, std::size_t index) const;

		virtual std::size_t change_parent_(tree *value, std::size_t index = static_cast<std::size_t>(-1));

		virtual bool remove_parent_();

		virtual void parent_changed_(tree *previous_parent, std::size_t previous_index);

		virtual bool validate_index_change_(std::size_t value) const;

		virtual std::size_t change_index_(std::size_t value);

		virtual void index_changed_(std::size_t previous);

		virtual std::size_t index_() const;

		virtual void set_previous_sibling_(object *target);

		virtual object *get_previous_sibling_() const;

		virtual void set_next_sibling_(object *target);

		virtual object *get_next_sibling_() const;

		tree *parent_;
		std::size_t m_index_;
	};
}
