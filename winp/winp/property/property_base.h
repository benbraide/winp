#pragma once

#include <functional>

#include "default_error_mapper.h"

namespace winp::prop{
	class base{
	public:
		using change_callback_type = std::function<bool(const base &, const void *, std::size_t)>;
		using setter_type = std::function<void(const base &, const void *, std::size_t)>;
		using getter_type = std::function<void(const base &, void *, std::size_t)>;

		using error_value_type = default_error_mapper::value_type;

		base();

		base(const base &) = delete;

		virtual ~base();

		base &operator =(const base &) = delete;

	protected:
		virtual void init_(change_callback_type changed_callback);

		virtual void init_(change_callback_type callback, setter_type setter, getter_type getter);

		virtual void change_(const void *value, std::size_t context = 0);

		virtual bool changed_(const void *value_ref, std::size_t context) const;

		virtual void get_value_(void *buf, std::size_t context = 0) const;

		virtual void throw_(error_value_type value = error_value_type::proper_does_not_support_action) const;

		change_callback_type changed_callback_;
	};

	template <class in_owner_type>
	class owned_base : public base{
	public:
		virtual ~owned_base() = default;

	protected:
		friend in_owner_type;

		virtual void init_(change_callback_type changed_callback) override{
			base::init_(changed_callback);
		}

		virtual void init_(change_callback_type callback, setter_type setter, getter_type getter) override{
			base::init_(callback, setter, getter);
		}
	};

	template <class value_type, class in_owner_type>
	class immediate_value : public owned_base<in_owner_type>{
	public:
		using base_type = owned_base<in_owner_type>;
		using owner_type = in_owner_type;
		using m_value_type = value_type;

		virtual ~immediate_value() = default;

	protected:
		friend in_owner_type;

		using base_type::changed_;

		virtual void change_(const void *value, std::size_t context = 0) override{
			if (!base_type::changed_(nullptr, context))
				return;

			m_value_ = *static_cast<const m_value_type *>(value);
			base_type::changed_(&m_value_, context);
		}

		virtual void typed_change_(const m_value_type &value){
			change_(&value);
		}

		virtual void get_value_(void *buf, std::size_t context = 0) const override{
			*static_cast<m_value_type *>(buf) = m_value_;
		}

		virtual const m_value_type &typed_get_value_() const{
			return m_value_;
		}

		virtual m_value_type &typed_get_value_(){
			return m_value_;
		}

		m_value_type m_value_;
	};

	template <class in_owner_type>
	class immediate_value<void, in_owner_type> : public owned_base<in_owner_type>{
	public:
		using base_type = owned_base<in_owner_type>;
		using owner_type = in_owner_type;
		using m_value_type = void;

		virtual ~immediate_value() = default;
	};

	template <class value_type, class in_owner_type>
	class proxy_value : public owned_base<in_owner_type>{
	public:
		using base_type = owned_base<in_owner_type>;
		using owner_type = in_owner_type;
		using m_value_type = value_type;

		using change_callback_type = base::change_callback_type;
		using setter_type = base::setter_type;
		using getter_type = base::getter_type;

		proxy_value() = default;

		virtual ~proxy_value() = default;

	protected:
		friend in_owner_type;

		virtual void init_(change_callback_type callback, setter_type setter, getter_type getter) override{
			base_type::init_(callback);
			setter_ = setter;
			getter_ = getter;
		}

		virtual void change_(const void *value, std::size_t context = 0) override{
			if (!base_type::changed_(nullptr, context))
				return;

			if (setter_ != nullptr)
				setter_(*this, value, context);
			else
				base_type::throw_();
		}

		virtual void typed_change_(const m_value_type &value){
			change_(&value);
		}

		virtual void get_value_(void *buf, std::size_t context = 0) const override{
			if (getter_ != nullptr)
				getter_(*this, buf, context);
			else
				base_type::throw_();
		}

		virtual m_value_type typed_get_value_() const{
			auto value = m_value_type();
			get_value_(&value);
			return value;
		}

		setter_type setter_;
		getter_type getter_;
	};

	template <class in_owner_type>
	class proxy_value<void, in_owner_type> : public owned_base<in_owner_type>{
	public:
		using base_type = owned_base<in_owner_type>;
		using owner_type = in_owner_type;
		using m_value_type = void;

		proxy_value() = default;

		virtual ~proxy_value() = default;

	protected:
		friend in_owner_type;
	};
}
