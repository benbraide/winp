#pragma once

#include "menu_wrapper.h"

namespace winp::menu{
	template <class base_type>
	class generic_collection_base : public base_type{
	public:
		using m_base_type = base_type;

		using item_ptr_type = std::shared_ptr<menu::item_component>;
		using list_type = std::list<item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection_base(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection_base() = default;

		template <typename item_type>
		item_type *insert(const std::function<bool(item_type &)> &callback, std::size_t index = static_cast<std::size_t>(-1)){
			if (callback == nullptr)
				return nullptr;//Callback required

			if (base_type::thread_.is_thread_context())
				return insert_(callback, index);

			base_type::thread_.queue.post([=]{
				insert_(callback, index);
			}, thread::queue::send_priority, base_type::id_);

			return nullptr;
		}

	protected:
		friend class thread::surface_manager;

		template <typename item_type>
		item_type *insert_(const std::function<bool(item_type &)> &callback, std::size_t index){
			auto item = std::make_shared<item_type>();
			if (item == nullptr || !callback(*item) || base_type::add_child_(*item, index) == static_cast<std::size_t>(-1))//Rejected
				return nullptr;

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

		using item_ptr_type = std::shared_ptr<menu::item_component>;
		using list_type = std::list<item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection() = default;

	protected:
		virtual void child_removed_(ui::object &child, std::size_t previous_index) override{
			for (auto it = item_list_.begin(); it != item_list_.end(); ++it){
				if (dynamic_cast<ui::object *>(it->get()) == &child){
					item_list_.erase(it);
					break;
				}
			}

			m_generic_base_type::template child_removed_(child, previous_index);
		}

		virtual void add_to_list_(item_ptr_type item) override{
			item_list_.push_back(item);
		}

		list_type item_list_;
	};

	template <>
	class generic_collection<wrapper> : public generic_collection_base<wrapper>{
	public:
		using m_generic_base_type = generic_collection_base<wrapper>;

		using item_ptr_type = std::shared_ptr<menu::item_component>;
		using list_type = std::list<item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection() = default;

	protected:
		virtual void add_to_list_(item_ptr_type item) override{
			item_list_.push_back(item);
		}
	};

	using collection = generic_collection<object>;
	using wrapper_collection = generic_collection<wrapper>;
}
