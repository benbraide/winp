#pragma once

#include <cstddef>

namespace winp::utility{
	template <class target_type, class... type_list>
	struct index_of_type;

	template <class target_type, class... type_list>
	struct index_of_type<target_type, target_type, type_list...> : std::integral_constant<std::size_t, 0>{};

	template <class target_type, class other_type, class... type_list>
	struct index_of_type<target_type, other_type, type_list...>
		: std::integral_constant<std::size_t, 1 + index_of_type<target_type, type_list...>::value>{};

	template <class... in_type_list>
	struct type_list{
		static constexpr std::size_t size = sizeof...(in_type_list);

		template <typename target_type>
		static constexpr std::size_t index_of = index_of_type<target_type, in_type_list...>::value;
	};
}
