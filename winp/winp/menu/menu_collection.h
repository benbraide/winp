#pragma once

#include "menu_wrapper.h"

namespace winp::menu{
	template <class base_type>
	class generic_collection_base : public base_type{
	public:
		using m_base_type = base_type;

		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::list<item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection_base(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection_base(){
			base_type::destruct_();
		}

		template <typename item_type>
		void add(const std::function<bool(item_type &)> &callback){
			if (callback == nullptr)
				return;//Callback required

			base_type::thread_.queue.post([=]{
				add_(callback);
			}, thread::queue::send_priority, base_type::id_);
		}

	protected:
		template <typename item_type>
		void add_(const std::function<bool(item_type &)> &callback){
			auto item = std::make_shared<item_type>(*this);
			if (callback(*item)){
				item->create();
				add_to_list_(item);
			}
			else//Rejected
				base_type::erase_child_(*item);
		}

		virtual void add_to_list_(item_ptr_type item) = 0;
	};

	template <class base_type>
	class generic_collection : public generic_collection_base<base_type>{
	public:
		using m_generic_base_type = generic_collection_base<base_type>;

		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::list<item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection(){
			m_generic_base_type::template destruct_();
		}

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

		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::list<item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection(){
			m_generic_base_type::destruct_();
		}

	protected:
		virtual void add_to_list_(item_ptr_type item) override{
			item_list_.push_back(item);
		}
	};

	using collection = generic_collection<object>;
	using wrapper_collection = generic_collection<wrapper>;
}
