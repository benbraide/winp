#pragma once

#include <memory>

#include "menu_item.h"

namespace winp::menu{
	class link : public item{
	public:
		using object_ptr_type = std::shared_ptr<object>;

		link();

		explicit link(thread::object &thread);

		explicit link(ui::tree &parent);

		virtual ~link();

		template <typename target_type, typename... args_types>
		target_type *create_popup(const std::function<bool(target_type &)> &callback, args_types... args){
			if (thread_.is_thread_context())//Inside thread context
				return create_popup_(callback, args...);

			if (callback == nullptr)
				return nullptr;//Callback required

			thread_.queue.post([=]{
				create_popup_(callback, args...);
			}, thread::queue::send_priority, id_);

			return nullptr;
		}

	protected:
		template <class> friend class menu::generic_collection_base;
		friend class menu::wrapper;

		virtual void child_removed_(ui::object &child, std::size_t previous_index) override;

		template <typename target_type, typename... args_types>
		target_type *create_popup_(const std::function<bool(target_type &)> &callback, args_types... args){
			auto target = std::make_shared<target_type>(thread_, args...);
			if (callback != nullptr && !callback(*target))
				return nullptr;

			add_child_(*target);
			target_ptr_ = target;

			return target.get();
		}

		object_ptr_type target_ptr_;
	};
}
