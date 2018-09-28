#pragma once

#include <variant>
#include <typeinfo>

#include "../utility/type_list.h"

#include "scalar_property.h"

namespace winp::prop{
	template <class in_owner_type, template <class, class> class value_holder_type, class... value_types>
	class variant : public value_holder_type<std::variant<value_types...>, in_owner_type>{
	public:
		using m_base_type = value_holder_type<std::variant<value_types...>, in_owner_type>;
		using m_type_list_type = utility::type_list<value_types...>;

		template <typename target_type>
		operator const target_type &() const{
			return std::get<target_type>(m_base_type::m_value_);
		}

		template <typename target_type>
		operator const target_type *() const{
			return (std::holds_alternative<target_type>(m_base_type::m_value_) ? &std::get<target_type>(m_base_type::m_value_) : nullptr);
		}

		template <typename target_type>
		operator target_type *(){
			return (std::holds_alternative<target_type>(m_base_type::m_value_) ? &std::get<target_type>(m_base_type::m_value_) : nullptr);
		}

		template <typename target_type>
		variant &operator =(const target_type &target){
			if (!m_base_type::changed_(nullptr, m_type_list_type::template index_of<target_type>))
				return *this;

			m_base_type::typed_get_value_() = target;
			m_base_type::changed_(&m_base_type::typed_get_value_(), m_type_list_type::template index_of<target_type>);

			return *this;
		}
	};

	template <class in_owner_type, class... value_types>
	class variant<in_owner_type, proxy_value, value_types...> : public proxy_value<void *, in_owner_type>{
	public:
		using m_base_type = proxy_value<void *, in_owner_type>;
		using m_type_list_type = utility::type_list<value_types...>;

		template <typename target_type>
		operator target_type() const{
			auto value = target_type();
			m_base_type::get_value_(&value, m_type_list_type::template index_of<target_type>);
			return value;
		}

		template <typename target_type>
		variant &operator =(const target_type &target){
			m_base_type::change_(&target, m_type_list_type::template index_of<target_type>);
			return *this;
		}
	};

	template <class in_owner_type>
	class variant<in_owner_type, immediate_value> : public immediate_value<void *, in_owner_type>{
	public:
		using m_base_type = immediate_value<void *, in_owner_type>;
		using m_type_list_type = utility::type_list<>;

		template <typename target_type>
		operator target_type *() const{
			return (target_type *)m_base_type::m_value_;
		}

		template <typename target_type>
		operator target_type &() const{
			return *operator target_type *();
		}

		template <typename target_type>
		variant &operator =(const target_type &target){
			if (!m_base_type::changed_(nullptr, 0u))
				return *this;

			*((target_type *)m_base_type::m_value_) = target;
			m_base_type::changed_(&m_base_type::m_value_, 0u);

			return *this;
		}
	};

	template <class in_owner_type>
	class variant<in_owner_type, proxy_value> : public proxy_value<void *, in_owner_type>{
	public:
		using m_base_type = proxy_value<void *, in_owner_type>;
		using m_type_list_type = utility::type_list<>;

		template <typename target_type>
		operator target_type() const{
			auto value = target_type();
			m_base_type::get_value_(&value, reinterpret_cast<std::size_t>(&typeid(target_type)));
			return value;
		}

		template <typename target_type>
		variant &operator =(const target_type &target){
			m_base_type::change_(&target, reinterpret_cast<std::size_t>(&typeid(target_type)));
			return *this;
		}
	};
}
