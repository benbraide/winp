#pragma once

#include <functional>

#include "default_error_mapper.h"

namespace winp::prop{
	class untyped_base{
	public:
		using error_value_type = default_error_mapper::value_type;

		virtual ~untyped_base();

	protected:
		virtual void change_(const void *value, std::size_t size = 0);

		void throw_(error_value_type value = error_value_type::proper_does_not_support_action) const;
	};

	template <class in_owner_type>
	class base : public untyped_base{
	public:
		using owner_type = in_owner_type;

		using change_callback_type = std::function<bool(const base &, const void *, std::size_t)>;
		using setter_type = std::function<void(const base &, const void *, std::size_t)>;
		using getter_type = std::function<void(const base &, void *, std::size_t)>;

		using error_value_type = default_error_mapper::value_type;

		virtual ~base() = default;

	protected:
		friend owner_type;

		virtual void init_(owner_type &owner, change_callback_type changed_callback){
			owner_ = &owner;
			changed_callback_ = changed_callback;
		}

		virtual void init_(owner_type &owner, change_callback_type callback, setter_type setter, getter_type getter){
			init_(owner, callback);
		}

		bool changed_(const void *value_ref, std::size_t size) const{
			return ((changed_callback_ == nullptr) ? true : changed_callback_(*this, value_ref, size));
		}

		owner_type *owner_ = nullptr;
		change_callback_type changed_callback_ = nullptr;
	};

	template <>
	class base<void> : public untyped_base{
	public:
		using owner_type = void;

		using change_callback_type = std::function<bool(const base &, const void *, std::size_t)>;
		using setter_type = std::function<void(const base &, const void *, std::size_t)>;
		using getter_type = std::function<void(const base &, void *, std::size_t)>;

		using error_value_type = default_error_mapper::value_type;

		virtual ~base() = default;

	protected:
		void init_(){}

		bool changed_(const void *, std::size_t) const{
			return true;
		}
	};

	template <class value_type, class in_owner_type>
	class immediate_value : public base<in_owner_type>{
	public:
		using base_type = base<in_owner_type>;
		using owner_type = typename base_type::owner_type;

		using m_value_type = value_type;
		using base_value_type = std::conditional_t<!std::is_pointer_v<value_type>, std::remove_const_t<std::remove_reference_t<value_type>>, std::remove_reference_t<value_type>>;
		using const_ref_value_type = const base_value_type &;
		using ref_value_type = base_value_type &;
		using ptr_value_type = std::remove_pointer_t<base_value_type> *;
		using const_ptr_value_type = const std::remove_pointer_t<base_value_type> *;

		template <typename... target_types>
		explicit immediate_value(target_types &&... targets)
			: m_value_(std::forward<target_types>(targets)...){}

		virtual ~immediate_value() = default;

	protected:
		friend in_owner_type;

		using base_type::changed_;

		bool changed_(std::size_t size = 0){
			return base_type::changed_(&m_value_, size);
		}

		void change_(const_ref_value_type value, std::size_t size = 0){
			if (base_type::changed_(nullptr, size)){
				m_value_ = value;
				changed_(size);
			}
		}

		virtual void change_(const void *value, std::size_t size = 0) override{
			change_(*static_cast<const base_value_type *>(value), size);
		}

		ref_value_type value_(std::size_t size = 0){
			return m_value_;
		}

		const_ref_value_type value_(std::size_t size = 0) const{
			return m_value_;
		}

		void value_(void *buf, std::size_t size = 0) const{
			*static_cast<ptr_value_type>(buf) = value_(size);
		}

		base_value_type m_value_;
	};

	template <class in_owner_type>
	class immediate_value<void, in_owner_type> : public base<in_owner_type>{
	public:
		using base_type = base<in_owner_type>;
		using owner_type = typename base_type::owner_type;

		using m_value_type = void;
		using base_value_type = void *;
		using const_ref_value_type = const base_value_type &;
		using ref_value_type = base_value_type & ;
		using ptr_value_type = std::remove_pointer_t<base_value_type> *;
		using const_ptr_value_type = const std::remove_pointer_t<base_value_type> *;

		virtual ~immediate_value() = default;
	};

	template <class value_type, class in_owner_type>
	class proxy_value : public base<in_owner_type>{
		public:
		using base_type = base<in_owner_type>;
		using owner_type = typename base_type::owner_type;
		using change_callback_type = typename base_type::change_callback_type;

		using m_value_type = value_type;
		using base_value_type = std::conditional_t<!std::is_pointer_v<value_type>, std::remove_const_t<std::remove_reference_t<value_type>>, value_type>;
		using const_ref_value_type = const base_value_type &;
		using ref_value_type = base_value_type &;
		using ptr_value_type = std::remove_pointer_t<base_value_type> *;
		using const_ptr_value_type = const std::remove_pointer_t<base_value_type> *;

		using setter_type = typename base_type::setter_type;
		using getter_type = typename base_type::getter_type;

		proxy_value() = default;

		explicit proxy_value(const value_type &){}

		template <typename setter_type, typename getter_type>
		proxy_value(setter_type setter, getter_type getter)
			: setter_(setter), getter_(getter){}

		virtual ~proxy_value() = default;

	protected:
		friend in_owner_type;

		virtual void init_(owner_type &owner, change_callback_type callback, setter_type setter, getter_type getter) override{
			base_type::init_(owner, callback);
			setter_ = setter;
			getter_ = getter;
		}

		void change_(const_ref_value_type value, std::size_t size = 0){
			change_(&value, size);
		}

		virtual void change_(const void *value, std::size_t size = 0) override{
			if (base_type::changed_(nullptr, size)){
				if (setter_ != nullptr)
					setter_(*this, value, size);
				else
					base_type::throw_();
			}
		}

		ref_value_type value_(std::size_t size = 0){
			return m_value_;
		}

		const_ref_value_type value_(std::size_t size = 0) const{
			if (getter_ != nullptr)
				getter_(*this, &m_value_, size);
			else
				base_type::throw_();
			return m_value_;
		}

		void value_(void *buf, std::size_t size = 0) const{
			if (getter_ != nullptr)
				getter_(*this, buf, size);
			else
				base_type::throw_(default_error_mapper::value_type::proper_has_no_getter);
		}

		setter_type setter_;
		getter_type getter_;
		mutable base_value_type m_value_ = base_value_type();
	};

	template <class value_type>
	class proxy_value<value_type, void> : public base<void>{
	public:
		using base_type = base<void>;
		using owner_type = base_type::owner_type;
		using change_callback_type = base_type::change_callback_type;

		using m_value_type = value_type;
		using base_value_type = std::conditional_t<!std::is_pointer_v<value_type>, std::remove_const_t<std::remove_reference_t<value_type>>, value_type>;
		using const_ref_value_type = const base_value_type &;
		using ref_value_type = base_value_type & ;
		using ptr_value_type = std::remove_pointer_t<base_value_type> *;
		using const_ptr_value_type = const std::remove_pointer_t<base_value_type> *;

		using setter_type = base_type::setter_type;
		using getter_type = base_type::getter_type;

		proxy_value() = default;

		explicit proxy_value(const value_type &){}

		virtual ~proxy_value() = default;

	protected:
		base_value_type value_(std::size_t size = 0) const{
			return base_value_type();
		}

		void value_(void *buf, std::size_t size = 0) const{
			*static_cast<ptr_value_type>(buf) = value_type();
		}

		setter_type setter_;
		getter_type getter_;
	};

	template <class in_owner_type>
	class proxy_value<void, in_owner_type> : public base<in_owner_type>{
	public:
		using base_type = base<in_owner_type>;
		using owner_type = typename base_type::owner_type;

		using m_value_type = void;
		using base_value_type = void *;
		using const_ref_value_type = const base_value_type &;
		using ref_value_type = base_value_type & ;
		using ptr_value_type = std::remove_pointer_t<base_value_type> *;
		using const_ptr_value_type = const std::remove_pointer_t<base_value_type> *;

		using setter_type = typename base_type::setter_type;
		using getter_type = typename base_type::getter_type;

		proxy_value() = default;

		template <typename setter_type, typename getter_type>
		proxy_value(setter_type, getter_type){}

		virtual ~proxy_value() = default;

	protected:
		friend in_owner_type;

		template <typename... arg_types>
		void init_(arg_types &&... args){
			base_type::init_(std::forward<arg_types>(args)...);
		}

		setter_type setter_;
		getter_type getter_;
	};
}
