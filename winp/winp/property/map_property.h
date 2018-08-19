#pragma once

#include <memory>

#include "../utility/type_list.h"

#include "property_base.h"

namespace winp::prop{
	template <class in_owner_type, class return_type, class... args_types>
	class map : public proxy_value<return_type, in_owner_type>{
	public:
		using m_value_holder_type = proxy_value<return_type, in_owner_type>;
		using m_base_type = proxy_value<return_type, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using change_callback_type = typename m_value_holder_type::change_callback_type;
		using setter_type = typename m_value_holder_type::setter_type;
		using getter_type = typename m_value_holder_type::getter_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		using m_type_list = utility::type_list<args_types...>;

		template <typename target_type>
		return_type operator [](const target_type &target){
			return get_<target_type>(target, std::bool_constant<m_type_list::size == 1u>());
		}

	protected:
		template <typename target_type>
		return_type get_(const target_type &target, std::true_type){
			using first_type = typename std::tuple_element<0, std::tuple<args_types...>>::type;
			return get0_<first_type>(target, std::bool_constant<std::is_pointer_v<first_type> && !std::is_pointer_v<target_type>>());
		}

		template <typename target_type>
		return_type get_(const target_type &target, std::false_type){
			auto value = std::make_pair(return_type(), target);
			change_(&value, m_type_list::template index_of<target_type>::value);
			return value.first;
		}

		template <typename first_type, typename target_type>
		return_type get0_(const target_type &target, std::true_type){
			auto value = std::make_pair(return_type(), &target);
			change_(&value);
			return value.first;
		}

		template <typename first_type, typename target_type>
		return_type get0_(const target_type &target, std::false_type){
			auto value = std::make_pair(return_type(), static_cast<first_type>(target));
			change_(&value);
			return value.first;
		}

		template <typename target_type>
		return_type get1_(const target_type &target, std::true_type){

		}

		template <typename target_type>
		return_type get1_(const target_type &target, std::false_type){

		}
	};

	template <class in_owner_type, class... args_types>
	class map<in_owner_type, void, args_types...> : public proxy_value<void, in_owner_type>{
	public:
		using m_value_holder_type = proxy_value<void, in_owner_type>;
		using m_base_type = proxy_value<void, in_owner_type>;

		using base_type = typename m_value_holder_type::base_type;
		using owner_type = typename m_value_holder_type::owner_type;

		using change_callback_type = typename m_value_holder_type::change_callback_type;
		using setter_type = typename m_value_holder_type::setter_type;
		using getter_type = typename m_value_holder_type::getter_type;

		using m_value_type = typename m_value_holder_type::m_value_type;
		using base_value_type = typename m_value_holder_type::base_value_type;
		using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;
		using ref_value_type = typename m_value_holder_type::ref_value_type;
		using ptr_value_type = typename m_value_holder_type::ptr_value_type;
		using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;

		using m_type_list = utility::type_list<args_types...>;

		template <typename target_type>
		void operator [](const target_type &target){
			get_<target_type>(target, std::bool_constant<m_type_list::size == 1u>());
		}

	protected:
		template <typename target_type>
		void get_(const target_type &target, std::true_type){
			auto value = static_cast<typename std::tuple_element<0, std::tuple<args_types...>>::type>(target);
			change_(&value);
		}

		template <typename target_type>
		void get_(const target_type &target, std::false_type){
			change_(&target, m_type_list::template index_of<target_type>::value);
		}
	};
}
