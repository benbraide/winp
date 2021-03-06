#pragma once

#include <memory>

#include "menu_item.h"

namespace winp::menu{
	class link : public item{
	public:
		using object_ptr_type = std::shared_ptr<object>;

		link();

		explicit link(bool);

		explicit link(thread::object &thread);

		link(thread::object &thread, bool);

		explicit link(ui::tree &parent);

		link(ui::tree &parent, bool);

		virtual ~link();

		template <typename target_type, typename... args_types>
		target_type *create_popup(const std::function<void(target_type &)> &callback, args_types... args){
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

		virtual void handle_child_inserted_event_(event::tree &e) override;

		template <typename target_type, typename... args_types>
		target_type *create_popup_(const std::function<void(target_type &)> &callback, args_types... args){
			auto target = std::make_shared<target_type>(thread_, args...);
			if (target == nullptr || add_child_(*target) == static_cast<std::size_t>(-1))
				return nullptr;//Rejected

			if (callback != nullptr)
				callback(*target);

			target->create();
			target_ptr_ = target;

			return target.get();
		}

		object_ptr_type target_ptr_;
	};
}
