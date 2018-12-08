#pragma once

#include "../event/event_manager.h"
#include "../utility/dynamic_list.h"
#include "../thread/thread_item.h"

#include "ui_modifier.h"

namespace winp::event{
	class object;
	class tree;
	class draw;
	class dispatcher;
}

namespace winp::message{
	class dispatcher;
	class mouse_dispatcher;
}

namespace winp::thread{
	class surface_manager;
}

namespace winp::menu{
	class item_component;
	class item;
	class group;
	class object;
	class separator;
}

namespace winp::non_window{
	class child;
}

namespace winp::ui{
	class tree;
	class window_surface;
	class hook;

	class object : public thread::item{
	public:
		using tree_change_info = event::tree::tree_change_info;

		using hook_list_type = std::map<int, std::shared_ptr<hook>>;
		using hook_map_type = std::unordered_map<unsigned int, hook_list_type>;

		object();

		explicit object(thread::object &thread);

		virtual ~object();

		virtual bool create(const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool destroy(const std::function<void(object &, bool)> &callback = nullptr);

		virtual HANDLE get_handle(const std::function<void(HANDLE)> &callback = nullptr) const;

		virtual bool is_ancestor(const tree &target, const std::function<void(bool)> &callback = nullptr) const;

		virtual std::size_t set_parent(tree *value, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual tree *get_parent(const std::function<void(tree *)> &callback = nullptr) const;

		template <typename target_type, typename before_type = void>
		target_type *get_first_ancestor_of(const std::function<void(tree *)> &callback = nullptr) const{
			if (is_thread_context()){
				auto result = get_first_ancestor_of_<target_type, before_type>();
				if (callback != nullptr)
					callback(result);
				return result;
			}

			if (callback != nullptr){
				use_context([=]{ callback(get_first_ancestor_of_<target_type, before_type>()); }, thread::queue::send_priority);
				return nullptr;
			}

			return execute_using_context([this]{ return get_first_ancestor_of_<target_type, before_type>(); }, thread::queue::send_priority);
		}

		virtual std::size_t set_index(std::size_t value, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual std::size_t get_index(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual bool set_previous_sibling(object *target, const std::function<void(object &, bool)> &callback = nullptr);

		virtual object *get_previous_sibling(const std::function<void(object *)> &callback = nullptr) const;

		virtual bool set_next_sibling(object *target, const std::function<void(object &, bool)> &callback = nullptr);

		virtual object *get_next_sibling(const std::function<void(object *)> &callback = nullptr) const;

		virtual utility::dynamic_list<tree, object> get_ancestors() const;

		virtual utility::dynamic_list<object, object> get_siblings() const;

		virtual const wchar_t *get_theme_name(const std::function<void(const wchar_t *)> &callback = nullptr) const;

		LRESULT send_message(UINT msg, const std::function<void(LRESULT)> &callback = nullptr);

		template <typename wparam_type = WPARAM>
		LRESULT send_message(UINT msg, wparam_type wparam, const std::function<void(LRESULT)> &callback = nullptr){
			return do_send_message_(msg, (WPARAM)wparam, 0, callback);
		}

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		LRESULT send_message(UINT msg, wparam_type wparam, lparam_type lparam, const std::function<void(LRESULT)> &callback = nullptr){
			return do_send_message_(msg, (WPARAM)wparam, (LPARAM)lparam, callback);
		}

		bool post_message(UINT msg, const std::function<void(bool)> &callback = nullptr);

		template <typename wparam_type = WPARAM>
		bool post_message(UINT msg, wparam_type wparam, const std::function<void(bool)> &callback = nullptr){
			return do_post_message_(msg, (WPARAM)wparam, 0, callback);
		}

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		bool post_message(UINT msg, wparam_type wparam, lparam_type lparam, const std::function<void(bool)> &callback = nullptr){
			return do_post_message_(msg, (WPARAM)wparam, (LPARAM)lparam, callback);
		}

		template <typename hook_type>
		hook_type *set_hook(const std::function<void(hook_type &)> &callback = nullptr){
			if (is_thread_context()){
				auto result = set_hook_<hook_type>();
				if (result != nullptr && callback != nullptr)
					callback(*result);
				return result;
			}

			use_context([=]{
				if (auto result = set_hook_<hook_type>(); result != nullptr && callback != nullptr)
					callback(*result);
			}, thread::queue::send_priority);

			return nullptr;
		}

		virtual bool remove_hook(unsigned int code, const std::function<void(object &, bool)> &callback = nullptr);

		virtual bool has_hook(unsigned int code, const std::function<void(bool)> &callback = nullptr) const;

		event::manager<object, event::tree> parent_change_event{ *this };
		event::manager<object, event::tree> index_change_event{ *this };

		event::manager<object, event::tree> parent_changed_event{ *this };
		event::manager<object, event::tree> index_changed_event{ *this };

	protected:
		friend class tree;
		friend class window_surface;

		friend class send_message;
		friend class post_message;

		friend class event::object;
		friend class event::tree;
		friend class event::draw;
		friend class event::dispatcher;
		friend class event::draw_item_dispatcher;

		friend class message::dispatcher;
		friend class message::mouse_dispatcher;
		friend class thread::surface_manager;

		friend class menu::item_component;
		friend class menu::item;
		friend class menu::separator;

		friend class menu::group;
		friend class menu::object;

		friend class non_window::child;
		friend class hook;

		void init_();

		virtual void destruct_() override;

		virtual bool create_();

		virtual bool destroy_();

		virtual void add_to_toplevel_(bool update = false);

		virtual void set_handle_(HANDLE value);

		virtual HANDLE get_handle_() const;

		virtual WNDPROC get_default_message_entry_() const;

		virtual bool is_ancestor_(const tree &target) const;

		virtual void set_parent_(tree *value);

		virtual tree *get_parent_() const;

		virtual std::size_t change_parent_(tree *value, std::size_t index = static_cast<std::size_t>(-1));

		virtual bool remove_parent_();

		virtual std::size_t change_index_(std::size_t value);

		virtual std::size_t get_index_() const;

		virtual bool set_previous_sibling_(object *target);

		virtual object *get_previous_sibling_() const;

		virtual bool set_next_sibling_(object *target);

		virtual object *get_next_sibling_() const;

		virtual const wchar_t *get_theme_name_() const;

		virtual LRESULT do_send_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(LRESULT)> &callback);

		virtual LRESULT send_message_(UINT msg, WPARAM wparam, LPARAM lparam);

		virtual bool do_post_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(bool)> &callback);

		virtual bool post_message_(UINT msg, WPARAM wparam, LPARAM lparam);

		template <typename hook_type>
		hook_type *set_hook_(){
			auto hook = std::make_shared<hook_type>(*this);
			if (hook == nullptr)
				return nullptr;

			auto code = hook->get_hook_code();
			if ((code & ui::hook::parent_size_change_hook_code) != 0u)
				hook_map_[ui::hook::parent_size_change_hook_code][hook->get_hook_index()] = hook;

			if ((code & ui::hook::child_size_change_hook_code) != 0u)
				hook_map_[ui::hook::child_size_change_hook_code][hook->get_hook_index()] = hook;

			call_hook_(code);
			return hook.get();
		}

		virtual bool remove_hook_(unsigned int code);

		virtual bool has_hook_(unsigned int code) const;

		virtual const hook_list_type *find_hook_(unsigned int code) const;

		virtual void call_hook_(unsigned int code);

		virtual std::size_t event_handlers_count_(event::manager_base &ev) const;

		virtual void fire_event_(event::manager_base &ev, event::object &e) const;

		virtual message::dispatcher *find_dispatcher_(UINT msg);

		virtual LRESULT dispatch_message_(UINT msg, WPARAM wparam, LPARAM lparam, bool call_default = false);

		template <typename target_type, typename before_type = void>
		target_type *get_first_ancestor_of_() const{
			return get_first_ancestor_of_<target_type, before_type>(std::bool_constant<std::is_void_v<before_type>>());
		}

		template <typename target_type, typename before_type>
		target_type *get_first_ancestor_of_(std::false_type) const{
			target_type *ancestor = nullptr;
			for (auto parent = get_parent_(); parent != nullptr; parent = get_parent_of_(*parent)){
				if (dynamic_cast<before_type *>(parent) != nullptr || (ancestor = dynamic_cast<target_type *>(parent)) != nullptr)
					break;
			}

			return ancestor;
		}

		template <typename target_type, typename before_type>
		target_type *get_first_ancestor_of_(std::true_type) const{
			target_type *ancestor = nullptr;
			for (auto parent = get_parent_(); parent != nullptr; parent = get_parent_of_(*parent)){
				if ((ancestor = dynamic_cast<target_type *>(parent)) != nullptr)
					break;
			}

			return ancestor;
		}

		static tree *get_parent_of_(const object &target);

		HANDLE handle_;
		tree *parent_;
		std::size_t index_;

		hook_map_type hook_map_;
		unsigned int called_hook_ = called_hook_ = ui::hook::nil_hook_code;

		utility::dynamic_list<tree, object> ancestor_list_;
		utility::dynamic_list<object, object> sibling_list_;
	};
}
