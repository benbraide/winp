#pragma once

#include <functional>

namespace winp::prop{
	template <class in_owner_type>
	class base{
	public:
		using owner_type = in_owner_type;
		typedef void (owner_type::*callback_type)(const base &, const void *, std::size_t);

		using setter_type = std::function<void(const base &, const void *, std::size_t)>;
		using getter_type = std::function<void(const base &, void *, std::size_t)>;

		virtual ~base() = default;

	protected:
		friend owner_type;

		virtual void init_(owner_type &owner, callback_type callback){
			owner_ = &owner;
			callback_ = callback;
		}

		virtual void init_(owner_type &owner, callback_type callback, setter_type setter, getter_type getter){
			init_(owner, callback);
		}

		void changed_(const void *value_ref, std::size_t size){
			if (owner_ != nullptr && callback_ != nullptr)
				(owner_->*callback_)(*this, value_ref, size);
		}

		owner_type *owner_ = nullptr;
		callback_type callback_ = nullptr;
	};

	template <>
	class base<void>{
	public:
		using owner_type = void;
		using callback_type = void;

		using setter_type = std::function<void(base &, const void *, std::size_t)>;
		using getter_type = std::function<void(base &, void *, std::size_t)>;

		virtual ~base() = default;

	protected:
		void init_(){}

		void changed_(){}

		template <typename value_type>
		void changed_(value_type &){}

		void changed_(const void *, std::size_t){}
	};

	template <class value_type, class in_owner_type>
	class immediate_value : public base<in_owner_type>{
	public:
		using base_type = base<in_owner_type>;
		using owner_type = typename base_type::owner_type;

		using m_value_type = value_type;
		using base_value_type = std::conditional_t<!std::is_pointer_v<value_type>, std::remove_const_t<std::remove_reference_t<value_type>>, value_type>;
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

		void changed_(std::size_t size = 0){
			base_type::changed_(&m_value_, size);
		}

		void change_(const_ref_value_type value, std::size_t size = 0){
			base_type::changed_(nullptr, size);
			m_value_ = value;
			changed_(size);
		}

		void change_(const void *value, std::size_t size = 0){
			change_(*static_cast<const_ptr_value_type>(value), size);
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

	protected:
		void changed_(){}

		void change_(){}

		void value_(){}
	};

	template <class value_type, class in_owner_type>
	class proxy_value : public base<in_owner_type>{
		public:
		using base_type = base<in_owner_type>;
		using owner_type = typename base_type::owner_type;
		using callback_type = typename base_type::callback_type;

		using m_value_type = value_type;
		using base_value_type = std::conditional_t<!std::is_pointer_v<value_type>, std::remove_const_t<std::remove_reference_t<value_type>>, value_type>;
		using const_ref_value_type = const base_value_type &;
		using ref_value_type = base_value_type &;
		using ptr_value_type = std::remove_pointer_t<base_value_type> *;
		using const_ptr_value_type = const std::remove_pointer_t<base_value_type> *;

		using setter_type = typename base_type::setter_type;
		using getter_type = typename base_type::getter_type;

		proxy_value() = default;

		template <typename setter_type, typename getter_type>
		proxy_value(setter_type setter, getter_type getter)
			: setter_(setter), getter_(getter){}

		virtual ~proxy_value() = default;

	protected:
		friend in_owner_type;

		virtual void init_(owner_type &owner, callback_type callback, setter_type setter, getter_type getter) override{
			base_type::init_(owner, callback);
			setter_ = setter;
			getter_ = getter;
		}

		void changed_(std::size_t size = 0){}

		void changed_(const void *value_ref, std::size_t size){
			base_type::changed_(value_ref, size);
		}

		void change_(const_ref_value_type value, std::size_t size = 0){
			base_type::changed_(nullptr, size);
			if (setter_ != nullptr)
				setter_(*this, const_cast<ptr_value_type *>(&value), size);
			else
				throw 0;
		}

		void change_(const void *value, std::size_t size = 0){
			change_(*static_cast<const_ptr_value_type>(value), size);
		}

		value_type value_(std::size_t size = 0) const{
			if (getter_ == nullptr)
				throw 0;

			value_type value;
			getter_(*this, &value, size);

			return value;
		}

		void value_(void *buf, std::size_t size = 0) const{
			*static_cast<ptr_value_type>(buf) = value_(size);
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

		proxy_value() = default;

		template <typename setter_type, typename getter_type>
		proxy_value(setter_type, getter_type){}

		virtual ~proxy_value() = default;

	protected:
		friend in_owner_type;

		template <typename setter_type, typename getter_type, typename... arg_types>
		void init_(setter_type, getter_type, arg_types &&... args){
			base_type::init_(std::forward<arg_types>(args)...);
		}

		void changed_(){}

		void change_(){}

		void value_() const{}
	};
}
