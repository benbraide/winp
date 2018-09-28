#pragma once

#include <memory>

#include "../utility/type_list.h"

#include "property_base.h"

namespace winp::prop{
	template <class first_type, class second_type, class in_owner_type>
	class map : public proxy_value<void *, in_owner_type>{
	public:
		using m_first_type = first_type;
		using m_second_type = second_type;

		using m_value_holder_type = proxy_value<void *, in_owner_type>;
		using m_base_type = proxy_value<void *, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;
		using m_value_type = typename m_value_holder_type::m_value_type;

		m_first_type operator [](const second_type &target) const{
			auto value = std::make_pair(m_first_type(), target);
			m_value_holder_type::get_value_(&value, 0u);
			return value;
		}

		template <typename dummy_type = first_type>
		std::enable_if_t<!std::is_same_v<dummy_type, second_type>, second_type> operator [](const m_first_type &target) const{
			auto value = std::make_pair(second_type(), target);
			m_value_holder_type::get_value_(&value, 1u);
			return value;
		}
	};

	template <class in_owner_type, class return_type, class... args_types>
	class multi_map : public proxy_value<return_type, in_owner_type>{
	public:
		using m_value_holder_type = proxy_value<return_type, in_owner_type>;
		using m_base_type = proxy_value<return_type, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;
		using m_value_type = typename m_value_holder_type::m_value_type;

		using change_callback_type = typename m_value_holder_type::change_callback_type;
		using setter_type = typename m_value_holder_type::setter_type;
		using getter_type = typename m_value_holder_type::getter_type;

		using m_type_list_type = utility::type_list<args_types...>;

		template <typename target_type>
		return_type operator [](const target_type &target){
			auto value = std::make_pair(return_type(), target);
			change_(&value, m_type_list_type::template index_of<target_type>);
			return value.first;
		}
	};

	template <class in_owner_type, class... args_types>
	class multi_map<in_owner_type, void, args_types...> : public proxy_value<void, in_owner_type>{
	public:
		using m_value_holder_type = proxy_value<void, in_owner_type>;
		using m_base_type = proxy_value<void, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;
		using m_value_type = typename m_value_holder_type::m_value_type;

		using change_callback_type = typename m_value_holder_type::change_callback_type;
		using setter_type = typename m_value_holder_type::setter_type;
		using getter_type = typename m_value_holder_type::getter_type;

		using m_type_list_type = utility::type_list<args_types...>;

		template <typename target_type>
		void operator [](const target_type &target){
			change_(&target, m_type_list_type::template index_of<target_type>);
		}
	};
}
