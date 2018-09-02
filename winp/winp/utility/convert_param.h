#pragma once

#include <cstdlib>

namespace winp::utility{
	template <class from_type, class to_type, class unused = void>
	struct convert_param{
		inline static to_type convert(from_type value){
			return (to_type)value;
		}
	};

	template <class to_type>
	struct convert_param<bool, to_type, std::enable_if_t<!std::is_same_v<to_type, bool>>>{
		inline static to_type convert(bool value){
			return to_type(value ? 1 : 0);
		}
	};

	template <class from_type>
	struct convert_param<from_type, bool, std::enable_if_t<!std::is_same_v<from_type, bool>>>{
		inline static bool convert(from_type value){
			return ((__int64)value != 0i64);
		}
	};
}
