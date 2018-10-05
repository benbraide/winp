#pragma once

#include "../event/event_manager.h"

#include "../utility/dynamic_list.h"
#include "../property/list_property.h"

#include "../thread/thread_item.h"

namespace winp::message{
	class dispatcher;
}

namespace winp::thread{
	class surface_manager;
}

namespace winp::ui{
	class object;
	class tree;

	class send_message{
	public:
		send_message();

		send_message(const send_message &copy);

		send_message &operator =(const send_message &copy);

		prop::scalar<HWND, send_message, prop::proxy_value> target;
		prop::scalar<LRESULT, send_message, prop::proxy_value> result;

		prop::scalar<unsigned int, send_message, prop::immediate_value> code;
		prop::scalar<WPARAM, send_message, prop::immediate_value> wparam;
		prop::scalar<LPARAM, send_message, prop::immediate_value> lparam;

	private:
		friend class object;

		void init_();

		HWND target_ = nullptr;
	};

	class post_message{
	public:
		post_message();

		post_message(const post_message &copy);

		post_message &operator =(const post_message &copy);

		prop::scalar<HWND, post_message, prop::proxy_value> target;
		prop::scalar<bool, post_message, prop::proxy_value> result;

		prop::scalar<unsigned int, post_message, prop::immediate_value> code;
		prop::scalar<WPARAM, post_message, prop::immediate_value> wparam;
		prop::scalar<LPARAM, post_message, prop::immediate_value> lparam;

	private:
		friend class object;

		void init_();

		HWND target_ = nullptr;
	};

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
		struct ancestor_change_info{
			tree *ancestor;	
			std::size_t index;
		};

		struct parent_change_info{
			bool is_changing;
			tree *current_parent;
			tree *new_parent;
			std::size_t index;
		};

		struct index_change_info{
			bool is_changing;
			std::size_t previous_index;
			std::size_t current_index;
		};

		struct sibling_change_info{
			object *sibling;
			std::size_t previous_index;
			std::size_t current_index;
		};

		explicit object(thread::object &thread);

		explicit object(tree &parent);

		virtual ~object();

		prop::scalar<HWND, object, prop::proxy_value> handle;
		prop::scalar<tree *, object, prop::proxy_value> parent;
		prop::scalar<std::size_t, object, prop::proxy_value> index;

		prop::scalar<object *, object, prop::proxy_value> previous_sibling;
		prop::scalar<object *, object, prop::proxy_value> next_sibling;

		prop::list<utility::dynamic_list<tree, object>, object, prop::immediate_value> ancestors;
		prop::list<utility::dynamic_list<object, object>, object, prop::immediate_value> siblings;

		event::manager<object, event::change<void, unsigned __int64>> change_event;

		static const unsigned __int64 ancestor_change_id		= (1ui64 << 0x00000000ui64);
		static const unsigned __int64 parent_change_id			= (1ui64 << 0x00000001ui64);
		static const unsigned __int64 index_change_id			= (1ui64 << 0x00000002ui64);
		static const unsigned __int64 sibling_change_id			= (1ui64 << 0x00000003ui64);

		static const unsigned __int64 last_object_change_id_bit	= 0x00000003ui64;

	protected:
		friend class tree;

		friend class send_message;
		friend class post_message;

		friend class message::dispatcher;
		friend class thread::surface_manager;

		void init_();

		virtual void do_request_(void *buf, const std::type_info &id) override;

		virtual void set_handle_(HWND value);

		virtual HWND get_handle_() const;

		virtual void set_parent_(tree *value);

		virtual tree *get_parent_() const;

		template <typename target_type>
		target_type *get_first_ancestor_of_() const{
			target_type *ancestor = nullptr;
			for (auto parent = get_parent_(); parent != nullptr; get_parent_of_(*parent)){
				if ((ancestor = dynamic_cast<target_type *>(parent)) != nullptr)
					break;
			}
			
			return ancestor;
		}

		virtual bool validate_parent_change_(tree *value, std::size_t index) const;

		virtual std::size_t change_parent_(tree *value, std::size_t index = static_cast<std::size_t>(-1));

		virtual bool remove_parent_();

		virtual void parent_changed_(tree *previous_parent, std::size_t previous_index);

		virtual bool validate_index_change_(std::size_t value) const;

		virtual std::size_t change_index_(std::size_t value);

		virtual void index_changed_(std::size_t previous);

		virtual std::size_t get_index_() const;

		virtual void set_previous_sibling_(object *target);

		virtual object *get_previous_sibling_() const;

		virtual void set_next_sibling_(object *target);

		virtual object *get_next_sibling_() const;

		virtual bool handles_message_(UINT msg) const;

		virtual bool handle_message_(message::basic &info);

		virtual void fire_ancestor_change_event_(tree *value, std::size_t index) const;

		virtual bool fire_parent_change_event_(bool is_changing, tree *current_value, tree *&value, std::size_t &index) const;

		virtual bool fire_index_change_event_(bool is_changing, std::size_t previous_value, std::size_t &value) const;

		virtual void fire_sibling_change_event_(object &sibling, std::size_t previous_index, std::size_t current_index) const;

		virtual void fire_event_(event::manager_base &ev, event::object &e) const;

		template <typename info_type>
		bool fire_change_event_(unsigned __int64 id, info_type &info, bool is_changing = false) const{
			event::change<void, unsigned __int64> e(id, info, const_cast<object *>(this));
			change_event.fire_(e);
			return (is_changing && !e.prevent_default);
		}

		static tree *get_parent_of_(const object &target);

		static message::dispatcher *find_dispatcher_(UINT msg);

		HWND handle_;
		tree *parent_;
		std::size_t index_;
	};
}
