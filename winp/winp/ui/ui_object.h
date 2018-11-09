#pragma once

#include "../event/event_manager.h"
#include "../utility/dynamic_list.h"
#include "../thread/thread_item.h"

namespace winp::event{
	class object;
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

namespace winp::ui{
	class tree;

	enum class event_id_type{
		create,
		destroy,
		close,
		change,
		size,
		move,
		show,
		hide,
		fail,
	};

	class object : public thread::item{
	public:
		object();

		explicit object(thread::object &thread);

		virtual ~object();

		virtual void create(const std::function<void(object &, bool)> &callback = nullptr);

		virtual void destroy(const std::function<void(object &, bool)> &callback = nullptr);

		virtual HWND get_handle(const std::function<void(HWND)> &callback = nullptr) const;

		virtual void set_parent(tree *value, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual tree *get_parent(const std::function<void(tree *)> &callback = nullptr) const;

		virtual void set_index(std::size_t value, const std::function<void(object &, bool, std::size_t)> &callback = nullptr);

		virtual std::size_t get_index(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual void set_previous_sibling(object *target, const std::function<void(object &, bool)> &callback = nullptr);

		virtual object *get_previous_sibling(const std::function<void(object *)> &callback = nullptr) const;

		virtual void set_next_sibling(object *target, const std::function<void(object &, bool)> &callback = nullptr);

		virtual object *get_next_sibling(const std::function<void(object *)> &callback = nullptr) const;

		virtual utility::dynamic_list<tree, object> get_ancestors(const std::function<void(utility::dynamic_list<tree, object>)> &callback = nullptr) const;

		virtual utility::dynamic_list<object, object> get_siblings(const std::function<void(utility::dynamic_list<object, object>)> &callback = nullptr) const;

		LRESULT send_message(UINT msg, const std::function<void(LRESULT)> &callback = nullptr);

		template <typename wparam_type = WPARAM>
		LRESULT send_message(UINT msg, wparam_type wparam, const std::function<void(LRESULT)> &callback = nullptr){
			return do_send_message_(msg, (WPARAM)wparam, 0, callback);
		}

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		LRESULT send_message(UINT msg, wparam_type wparam, lparam_type lparam, const std::function<void(LRESULT)> &callback = nullptr){
			return do_send_message_(msg, (WPARAM)wparam, (LPARAM)lparam, callback);
		}

		void post_message(UINT msg, const std::function<void(bool)> &callback = nullptr);

		template <typename wparam_type = WPARAM>
		void post_message(UINT msg, wparam_type wparam, const std::function<void(bool)> &callback = nullptr){
			do_post_message_(msg, (WPARAM)wparam, 0, callback);
		}

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		void post_message(UINT msg, wparam_type wparam, lparam_type lparam, const std::function<void(bool)> &callback = nullptr){
			do_post_message_(msg, (WPARAM)wparam, (LPARAM)lparam, callback);
		}

	protected:
		friend class tree;

		friend class send_message;
		friend class post_message;

		friend class event::object;
		friend class event::draw;
		friend class event::dispatcher;

		friend class message::dispatcher;
		friend class message::mouse_dispatcher;
		friend class thread::surface_manager;

		void init_();

		virtual bool create_();

		virtual bool destroy_();

		virtual void set_handle_(HWND value);

		virtual HWND get_handle_() const;

		virtual void set_parent_(tree *value);

		virtual tree *get_parent_() const;

		virtual bool validate_parent_change_(tree *value, std::size_t index) const;

		virtual std::size_t change_parent_(tree *value, std::size_t index = static_cast<std::size_t>(-1));

		virtual bool remove_parent_();

		virtual void parent_changed_(tree *previous_parent, std::size_t previous_index);

		virtual bool validate_index_change_(std::size_t value) const;

		virtual std::size_t change_index_(std::size_t value);

		virtual void index_changed_(std::size_t previous);

		virtual std::size_t get_index_() const;

		virtual bool set_previous_sibling_(object *target);

		virtual object *get_previous_sibling_() const;

		virtual bool set_next_sibling_(object *target);

		virtual object *get_next_sibling_() const;

		virtual LRESULT do_send_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(LRESULT)> &callback);

		virtual LRESULT send_message_(UINT msg, WPARAM wparam, LPARAM lparam);

		virtual void do_post_message_(UINT msg, WPARAM wparam, LPARAM lparam, const std::function<void(bool)> &callback);

		virtual bool post_message_(UINT msg, WPARAM wparam, LPARAM lparam);

		virtual std::size_t event_handlers_count_(event::manager_base &ev) const;

		virtual void fire_event_(event::manager_base &ev, event::object &e) const;

		message::dispatcher *find_dispatcher_(UINT msg);

		template <typename target_type>
		target_type *get_first_ancestor_of_() const{
			target_type *ancestor = nullptr;
			for (auto parent = get_parent_(); parent != nullptr; get_parent_of_(*parent)){
				if ((ancestor = dynamic_cast<target_type *>(parent)) != nullptr)
					break;
			}

			return ancestor;
		}

		static tree *get_parent_of_(const object &target);

		HWND handle_;
		tree *parent_;
		std::size_t index_;

		utility::dynamic_list<tree, object> ancestor_list_;
		utility::dynamic_list<object, object> sibling_list_;
	};
}
