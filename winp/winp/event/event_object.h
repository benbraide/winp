#pragma once

#include "../utility/windows.h"
#include "../property/scalar_property.h"
#include "../property/variant_property.h"
#include "../property/pair_property.h"

namespace winp::thread{
	class item;
	class object;
}

namespace winp::ui{
	class object;
}

namespace winp::event{
	template <class owner_type, class object_type>
	class manager;

	class object{
	public:
		struct state_type{
			static constexpr unsigned int nil						= (0 << 0x0000);
			static constexpr unsigned int default_prevented			= (1 << 0x0000);
			static constexpr unsigned int propagation_stopped		= (1 << 0x0001);
			static constexpr unsigned int result_set				= (1 << 0x0004);
		};

		explicit object(ui::object *target);

		virtual ~object();

		prop::scalar<ui::object *, object, prop::proxy_value> owner;
		prop::scalar<ui::object *, object, prop::proxy_value> target;

		prop::scalar<bool, object, prop::proxy_value> prevent_default;
		prop::scalar<bool, object, prop::proxy_value> stop_propagation;

	protected:
		friend class ui::object;
		friend class thread::object;
		template <class, class> friend class manager;

		bool bubble_();

		ui::object *owner_;
		ui::object *target_;
		unsigned int state_;
	};

	class message : public object{
	public:
		struct info_type{
			unsigned int code;
			WPARAM wparam;
			LPARAM lparam;
		};

		message(ui::object *target, const info_type &info);

		virtual ~message();

	protected:
		friend class ui::object;
		friend class thread::object;
		template <class, class> friend class manager;

		virtual LRESULT get_result_() const;

		info_type info_;
	};

	template <class base_type, class result_type>
	class typed : public base_type{
	public:
		using m_base_type = base_type;
		using m_result_type = result_type;

		template <typename... arg_types>
		explicit typed(arg_types &&... args)
			: base_type(std::forward<arg_types>(args)...){
			auto setter = [this](const prop::base &prop, const void *value, std::size_t context){
				if (&prop == &result){
					m_base_type::state_ |= message::state_type::result_set;
					result_ = *static_cast<const m_result_type *>(value);
				}
			};

			auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
				if (&prop == &result)
					*static_cast<m_result_type *>(buf) = result_;
			};

			result.init_(nullptr, setter, getter);
		}

		virtual ~typed() = default;

		prop::scalar<m_result_type, typed, prop::proxy_value> result;

	protected:
		friend class ui::object;
		friend class thread::object;
		template <class, class> friend class manager;

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

	template <class result_type, class id_type>
	class change : public typed<object, result_type>{
	public:
		using m_typed_base_type = typed<object, result_type>;
		using m_result_type = result_type;
		using m_id_type = id_type;

		template <typename info_type, typename... arg_types>
		change(const id_type &id, info_type &info, arg_types &&... args)
			: m_typed_base_type(std::forward<arg_types>(args)...), id_(id){
			init_(&info);
		}

		template <typename info_type, typename... arg_types>
		change(const id_type &id, info_type *info, arg_types &&... args)
			: m_typed_base_type(std::forward<arg_types>(args)...), id_(id){
			init_(info);
		}

		prop::scalar<id_type, change, prop::proxy_value> id;
		prop::variant<change, prop::immediate_value> info;

	protected:
		template <typename info_type>
		void init_(info_type *info){
			auto getter = [this](const prop::base &prop, void *buf, std::size_t context){
				if (&prop == &id)
					*static_cast<id_type *>(buf) = id_;
			};

			id.init_(nullptr, nullptr, getter);
			this->info.m_value_ = (void *)info;
		}

		id_type id_;
	};

	class mouse : public message{
	public:
		using m_point_type = utility::point<int>;

		enum class button_type{
			nil,
			left,
			middle,
			right,
		};

		mouse(ui::object *target, const info_type &info, const m_point_type &offset, button_type button);

		virtual ~mouse();

		prop::scalar<m_point_type, mouse, prop::proxy_value> position;
		prop::scalar<m_point_type, mouse, prop::proxy_value> offset;
		prop::scalar<button_type, mouse, prop::proxy_value> button;

	protected:
		virtual m_point_type get_position_() const;

		m_point_type offset_;
		button_type button_;
	};
}
