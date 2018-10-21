#pragma once

#include "../utility/windows.h"

namespace winp::message{
	class dispatcher;
	class draw_dispatcher;
}

namespace winp::thread{
	class item;
	class object;
}

namespace winp::ui{
	class object;
}

namespace winp::event{
	enum class event_result_type{
		nil,
		result_set,
		prevent_default,
	};

	template <class owner_type, class object_type>
	class manager;

	class object{
	public:
		struct state_type{
			static constexpr unsigned int nil						= (0 << 0x0000);
			static constexpr unsigned int default_prevented			= (1 << 0x0000);
			static constexpr unsigned int propagation_stopped		= (1 << 0x0001);
			static constexpr unsigned int result_set				= (1 << 0x0002);
		};

		explicit object(thread::object &thread);

		explicit object(ui::object &target);

		object(ui::object &target, ui::object &context);

		virtual ~object();

		virtual ui::object *get_target() const;

		virtual ui::object *get_context() const;

		template <typename value_type>
		void set_result(value_type value){
			set_result((LRESULT)value);
		}

		virtual void set_result(LRESULT value);

		virtual void set_result(bool value);

		virtual LRESULT get_result() const;

		virtual void prevent_default();

		virtual void do_default();

		virtual void stop_propagation();

	protected:
		friend class ui::object;
		friend class thread::object;
		friend class winp::message::dispatcher;
		template <class, class> friend class manager;

		virtual void set_result_(LRESULT value);

		virtual LRESULT get_result_() const;

		virtual bool bubble_();

		virtual void do_default_();

		virtual bool default_prevented_() const;

		virtual bool propagation_stopped_() const;

		virtual bool result_set_() const;

		ui::object *target_;
		ui::object *context_;

		thread::object &thread_;
		unsigned int state_;
	};

	class message : public object{
	public:
		struct info_type{
			unsigned int code;
			WPARAM wparam;
			LPARAM lparam;
		};

		message(ui::object &target, const info_type &info);

		message(ui::object &target, ui::object &context, const info_type &info);

		virtual ~message();

	protected:
		friend class ui::object;
		friend class thread::object;
		template <class, class> friend class manager;

		info_type info_;
	};

	template <class base_type, class result_type>
	class typed : public base_type{
	public:
		using m_base_type = base_type;
		using m_result_type = result_type;

		template <typename... arg_types>
		explicit typed(arg_types &&... args)
			: base_type(std::forward<arg_types>(args)...){}

		virtual ~typed() = default;

	protected:
		friend class ui::object;
		friend class thread::object;
		template <class, class> friend class manager;

		virtual void set_result_(LRESULT value) override{
			result_ = (m_result_type)value;
			base_type::state_ |= object::state_type::result_set;
		}

		virtual LRESULT get_result_() const override{
			return (LRESULT)result_;
		}

		m_result_type result_ = m_result_type();
	};

	template <class base_type>
	class typed<base_type, void> : public base_type{
	public:
		using m_base_type = base_type;
		using m_result_type = void;

		template <typename... arg_types>
		explicit typed(arg_types &&... args)
			: base_type(std::forward<arg_types>(args)...){}

		virtual ~typed() = default;

	protected:
		friend class ui::object;
		friend class thread::object;
		template <class, class> friend class manager;
	};

	class draw : public message{
	public:
		using m_rect_type = RECT;

		draw(ui::object &target, const info_type &info);

		draw(ui::object &target, ui::object &context, const info_type &info);

		virtual ~draw();

		virtual ID2D1RenderTarget *get_drawer();

		virtual ID2D1SolidColorBrush *get_color_brush();

		virtual HDC get_device();

		virtual m_rect_type get_region();

		virtual bool erase_background();

	protected:
		friend class winp::message::draw_dispatcher;

		virtual void set_target_(ui::object *target, POINT &offset);

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

	class mouse : public message{
	public:
		using m_point_type = POINT;

		enum class button_type{
			nil,
			left,
			middle,
			right,
		};

		mouse(ui::object &target, const info_type &info, const m_point_type &offset, button_type button);

		mouse(ui::object &target, ui::object &context, const info_type &info, const m_point_type &offset, button_type button);

		virtual ~mouse();

		virtual m_point_type get_position() const;

		virtual m_point_type get_offset() const;

		virtual button_type get_button() const;

	protected:
		virtual m_point_type get_position_() const;

		m_point_type offset_;
		button_type button_;
	};
}
