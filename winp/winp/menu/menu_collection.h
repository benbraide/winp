#pragma once

#include "menu_wrapper.h"

namespace winp::menu{
	template <class base_type>
	class generic_collection_base : public base_type{
	public:
		using m_base_type = base_type;

		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::unordered_map<menu::component *, item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection_base(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection_base() = default;

		template <typename item_type>
		item_type *insert(const std::function<bool(item_type &)> &callback, std::size_t index = static_cast<std::size_t>(-1)){
			if (base_type::thread_.is_thread_context())
				return insert_(callback, index);

			if (callback == nullptr)//Callback required
				return nullptr;

			base_type::thread_.queue.post([=]{
				insert_(callback, index);
			}, thread::queue::send_priority, base_type::id_);

			return nullptr;
		}

	protected:
		friend class thread::surface_manager;

		template <typename item_type>
		item_type *insert_(const std::function<bool(item_type &)> &callback, std::size_t index){
			auto item = std::make_shared<item_type>(base_type::thread_);
			if (item == nullptr || (callback != nullptr && !callback(*item)) || base_type::add_child_(*item, index) == static_cast<std::size_t>(-1))
				return nullptr;//Rejected

			item->create();
			add_to_list_(item);

			return item.get();
		}

		virtual void add_to_list_(item_ptr_type item) = 0;
	};

	template <class base_type>
	class generic_collection : public generic_collection_base<base_type>{
	public:
		using m_generic_base_type = generic_collection_base<base_type>;

		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::unordered_map<menu::component *, item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection() = default;

	protected:
		virtual void child_removed_(ui::object &child, std::size_t previous_index) override{
			if (!item_list_.empty())
				item_list_.erase(dynamic_cast<menu::component *>(&child));
			m_generic_base_type::template child_removed_(child, previous_index);
		}

		virtual void add_to_list_(item_ptr_type item) override{
			item_list_[item.get()] = item;
		}

		list_type item_list_;
	};

	template <>
	class generic_collection<wrapper> : public generic_collection_base<wrapper>{
	public:
		using m_generic_base_type = generic_collection_base<wrapper>;

		using item_ptr_type = std::shared_ptr<menu::component>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection() = default;

	protected:
		virtual void add_to_list_(item_ptr_type item) override{
			item_list_[item.get()] = item;
		}
	};

	using collection = generic_collection<object>;
	using wrapper_collection = generic_collection<wrapper>;

	using group_collection = generic_collection<group>;
	using radio_group_collection = generic_collection<radio_group>;
}
