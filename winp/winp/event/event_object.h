#pragma once

#include <bitset>
#include <memory>
#include <functional>

#include "../utility/windows.h"

namespace winp::message{
	class dispatcher;
	class draw_dispatcher;
	class mouse_dispatcher;
	class key_dispatcher;
}

namespace winp::thread{
	class item;
	class object;
}

namespace winp::ui{
	class object;
	class tree;
	class visible_surface;
}

namespace winp::menu{
	class object;

	template <class base_type>
	class generic_collection;
}

namespace winp::event{
	class draw_dispatcher;
	class unhandled_handler;
	class draw_handler;

	enum class event_result_type{
		nil,
		result_set,
		prevent_default,
	};

	template <class owner_type, class object_type, class group_type = void>
	class manager;

	class object{
	public:
		using callback_type = std::function<void(object &)>;
		using info_type = MSG;

		struct state_type{
			static constexpr unsigned int nil						= (0 << 0x0000);
			static constexpr unsigned int default_prevented			= (1 << 0x0000);
			static constexpr unsigned int propagation_stopped		= (1 << 0x0001);
			static constexpr unsigned int result_set				= (1 << 0x0002);
		};

		object(thread::object &thread, const callback_type &default_handler, const info_type &info);

		object(ui::object &target, const callback_type &default_handler, const info_type &info);

		object(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info);

		virtual ~object();

		virtual ui::object *get_target() const;

		virtual ui::object *get_context() const;

		virtual const info_type *get_info() const;

		template <typename value_type>
		void set_result(value_type value, bool always_set = true){
			set_result((LRESULT)value, always_set);
		}

		virtual void set_result(LRESULT value, bool always_set = true);

		virtual void set_result(bool value, bool always_set = true);

		virtual LRESULT get_result() const;

		virtual void prevent_default();

		virtual void do_default();

		virtual void stop_propagation();

	protected:
		friend class ui::object;
		friend class thread::object;
		friend class winp::message::dispatcher;
		template <class, class, class> friend class manager;

		virtual void set_result_(LRESULT value, bool always_set);

		virtual LRESULT get_result_() const;

		virtual void set_context_(ui::object &value);

		virtual bool bubble_();

		template <typename target_type>
		bool bubble_to_type_(){
			while (bubble_()){
				if (dynamic_cast<target_type *>(context_) != nullptr)
					return true;
			}

			return false;
		}

		virtual void do_default_();

		virtual bool default_prevented_() const;

		virtual bool propagation_stopped_() const;

		virtual bool result_set_() const;

		ui::object *target_;
		ui::object *context_;

		thread::object &thread_;
		unsigned int state_;

		callback_type default_handler_;
		LRESULT result_;
		info_type info_;
	};

	class tree : public object{
	public:
		tree(ui::object &target, const callback_type &default_handler, const info_type &info);

		tree(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info);

		virtual ~tree();

		virtual ui::tree *get_previous_parent() const;

		virtual ui::tree *get_current_parent() const;

		virtual std::size_t get_previous_index() const;

		virtual std::size_t get_current_index() const;

	protected:
		void init_();

		ui::tree *previous_parent_;
		ui::tree *current_parent_;

		std::size_t previous_index_;
		std::size_t current_index_;
	};

	class draw : public object{
	public:
		using m_rect_type = RECT;

		draw(ui::object &target, const callback_type &default_handler, const info_type &info);

		draw(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info);

		virtual ~draw();

		virtual ID2D1RenderTarget *get_drawer();

		virtual ID2D1SolidColorBrush *get_color_brush();

		virtual HDC get_device();

		virtual m_rect_type get_region();

		virtual bool erase_background();

	protected:
		friend class ui::visible_surface;
		friend class winp::message::draw_dispatcher;

		friend class draw_dispatcher;
		friend class unhandled_handler;
		friend class draw_handler;

		virtual void set_context_(ui::object &value) override;

		virtual void begin_();

		virtual ID2D1RenderTarget *get_drawer_();

		virtual ID2D1SolidColorBrush *get_color_brush_();

		virtual HDC get_device_();

		virtual m_rect_type get_region_();

		virtual bool erase_background_();

		ID2D1DCRenderTarget *drawer_ = nullptr;
		ID2D1SolidColorBrush *color_brush_ = nullptr;

		PAINTSTRUCT struct_{};
		POINT current_offset_{};

		int initial_device_state_id_ = -1;
		std::function<void()> cleaner_;
	};

	class cursor : public object{
	public:
		cursor(ui::object &target, const callback_type &default_handler, const info_type &info);

		cursor(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info);

		virtual ~cursor();

		virtual WORD get_hit_target() const;

		virtual WORD get_mouse_button() const;
	};

	class mouse : public object{
	public:
		using m_point_type = POINT;

		enum class button_type{
			nil,
			left,
			middle,
			right,
		};

		mouse(ui::object &target, const callback_type &default_handler, const info_type &info, const m_point_type &offset, button_type button);

		mouse(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info, const m_point_type &offset, button_type button);

		virtual ~mouse();

		virtual m_point_type get_position() const;

		virtual m_point_type get_offset() const;

		virtual button_type get_button() const;

	protected:
		friend class winp::message::mouse_dispatcher;

		virtual m_point_type get_position_() const;

		m_point_type offset_;
		button_type button_;
	};

	class key : public object{
	public:
		class keyboard_state{
		public:
			explicit keyboard_state(key &e);

			bool check_state(BYTE key) const;

			bool left_shift_pressed() const;

			bool right_shift_pressed() const;

			bool shift_pressed() const;

			bool left_ctrl_pressed() const;

			bool right_ctrl_pressed() const;

			bool ctrl_pressed() const;

			bool left_alt_pressed() const;

			bool right_alt_pressed() const;

			bool alt_pressed() const;

			bool left_win_pressed() const;

			bool right_win_pressed() const;

			bool win_pressed() const;

			bool caps_lock_on() const;

			bool num_lock_on() const;

			bool scroll_lock_on() const;

			bool insert_on() const;

		private:
			void retrieve_states_() const;

			key &e_;
		};

		key(ui::object &target, const callback_type &default_handler, const info_type &info);

		key(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info);

		virtual ~key();

		virtual unsigned short get_code() const;

		virtual wchar_t get_char() const;

		virtual WORD get_repeat_count() const;

		virtual bool is_char() const;

		virtual bool is_down() const;

		virtual bool is_first_down() const;

		virtual bool is_being_released() const;

		virtual bool is_extended() const;

		virtual const keyboard_state &get_keyboard_state() const;

	protected:
		friend class winp::message::key_dispatcher;

		keyboard_state keyboard_state_;

		static thread_local bool keyboard_states_retrieved_;
		static thread_local BYTE keyboard_states_[0x100];
	};

	class context_menu_prefix : public object{
	public:
		context_menu_prefix(ui::object &target, const callback_type &default_handler, const info_type &info);

		context_menu_prefix(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info);

		virtual ~context_menu_prefix();

		virtual POINT get_position() const;
	};

	class context_menu : public context_menu_prefix{
	public:
		using menu_type = menu::generic_collection<menu::object>;

		context_menu(ui::object &target, const callback_type &default_handler, const info_type &info);

		context_menu(ui::object &target, ui::object &context, const callback_type &default_handler, const info_type &info);

		virtual ~context_menu();

		virtual menu_type &get_menu();
	};
}
