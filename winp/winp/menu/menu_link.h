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

		link(ui::tree &parent, bool);

		virtual ~link();

		template <typename target_type, typename... args_types>
		void set_target(const std::function<bool(target_type &)> &callback, args_types... args){
			if (callback == nullptr)
				return;//Callback required

			thread_.queue.post([=]{
				set_target_(callback, args...);
			}, thread::queue::send_priority, id_);
		}

	protected:
		friend class wrapper;

		virtual void child_removed_(ui::object &child, std::size_t previous_index) override;

		template <typename target_type, typename... args_types>
		void set_target_(const std::function<bool(target_type &)> &callback, args_types... args){
			auto target = std::make_shared<target_type>(thread_, args...);
			if (callback(*target)){
				add_child_(*target);
				target_ptr_ = target;
			}
		}

		object_ptr_type target_ptr_;
	};
}
