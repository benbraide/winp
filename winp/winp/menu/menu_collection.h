#pragma once

#include "menu_wrapper.h"

namespace winp::menu{
	template <class base_type>
	class generic_collection;

	template <class base_type>
	class generic_collection_base : public base_type{
	public:
		using m_base_type = base_type;

		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::list<item_ptr_type>;
		using map_type = std::unordered_map<menu::component *, item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection_base(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection_base() = default;

		template <typename item_type>
		item_type *insert(const std::function<void(item_type &)> &callback, std::size_t index = static_cast<std::size_t>(-1)){
			if (base_type::thread_.is_thread_context())
				return insert_(callback, index);

			if (callback == nullptr)//Callback required
				return nullptr;

			base_type::thread_.queue.post([=]{
				insert_(callback, index);
			}, thread::queue::send_priority, base_type::id_);

			return nullptr;
		}

		virtual menu::item *insert_item(const std::wstring &label, const std::function<void(menu::item &)> &callback = nullptr, std::size_t index = static_cast<std::size_t>(-1)){
			return insert<menu::item>([=](menu::item &item){
				item.set_label(label);
				if (callback != nullptr)
					callback(item);
			}, index);
		}

		virtual menu::item *insert_item(const std::wstring &label, const std::function<void(event::object &)> &on_select, std::size_t index = static_cast<std::size_t>(-1)){
			return insert<menu::item>([=](menu::item &item){
				item.set_label(label);
				if (on_select != nullptr)
					item.select_event += on_select;
			}, index);
		}

		virtual menu::link *insert_link(const std::wstring &label, const std::function<void(menu::generic_collection<object> &)> &callback = nullptr, std::size_t index = static_cast<std::size_t>(-1)){
			return insert<menu::link>([=](menu::link &item){
				item.set_label(label);
				item.create_popup<menu::generic_collection<object>>(callback);
			}, index);
		}

		virtual menu::check_item *insert_check_item(const std::wstring &label, const std::function<void(menu::check_item &)> &callback = nullptr, std::size_t index = static_cast<std::size_t>(-1)){
			return insert<menu::check_item>([=](menu::check_item &item){
				item.set_label(label);
			}, index);
		}

		virtual menu::check_item *insert_check_item(const std::wstring &label, const std::function<void(event::object &)> &on_check, const std::function<void(event::object &)> &on_uncheck, std::size_t index = static_cast<std::size_t>(-1)){
			return insert<menu::check_item>([=](menu::check_item &item){
				item.set_label(label);
				if (on_check != nullptr)
					item.check_event += on_check;

				if (on_uncheck != nullptr)
					item.uncheck_event += on_uncheck;
			}, index);
		}

		virtual menu::separator *insert_separator(const std::function<void(menu::separator &)> &callback = nullptr, std::size_t index = static_cast<std::size_t>(-1)){
			return insert<menu::separator>(callback, index);
		}

	protected:
		friend class thread::surface_manager;

		template <typename item_type>
		item_type *insert_(const std::function<void(item_type &)> &callback, std::size_t index){
			auto item = std::make_shared<item_type>(base_type::thread_);
			if (item == nullptr || base_type::add_child_(*item, index) == static_cast<std::size_t>(-1))
				return nullptr;//Rejected

			if (callback != nullptr)
				callback(*item);

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
		using list_type = std::list<item_ptr_type>;
		using map_type = std::unordered_map<menu::component *, item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection() = default;

	protected:
		virtual void handle_child_inserted_event_(event::tree &e) override{
			if (!marked_items_.empty())
				marked_items_.clear();

			m_generic_base_type::handle_child_inserted_event_(e);
		}

		virtual void handle_child_removed_event_(event::tree &e) override{
			if (!item_map_.empty()){
				if (auto it = item_map_.find(dynamic_cast<menu::component *>(e.get_target())); it != item_map_.end()){
					marked_items_.push_back(it->second);
					item_map_.erase(it);
				}
			}

			m_generic_base_type::handle_child_removed_event_(e);
		}

		virtual void add_to_list_(item_ptr_type item) override{
			item_map_[item.get()] = item;
		}

		map_type item_map_;
		list_type marked_items_;
	};

	template <>
	class generic_collection<wrapper> : public generic_collection_base<wrapper>{
	public:
		using m_generic_base_type = generic_collection_base<wrapper>;

		using item_ptr_type = std::shared_ptr<menu::component>;
		using list_type = std::list<item_ptr_type>;
		using map_type = std::unordered_map<menu::component *, item_ptr_type>;

		template <typename... args_types>
		explicit generic_collection(args_types &&... args)
			: m_generic_base_type(std::forward<args_types>(args)...){}

		virtual ~generic_collection() = default;

	protected:
		virtual void add_to_list_(item_ptr_type item) override{
			item_map_[item.get()] = item;
		}
	};

	using collection = generic_collection<object>;
	using wrapper_collection = generic_collection<wrapper>;

	using group_collection = generic_collection<group>;
	using radio_group_collection = generic_collection<radio_group>;
}
