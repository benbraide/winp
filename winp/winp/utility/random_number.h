#pragma once

#include "random_generator.h"

#ifdef max
#undef max
#endif // max

#ifdef min
#undef min
#endif // min


namespace winp::utility{
	template <template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class random_number : public random_generator<distribution_type, engine_type, seeder_type>{
	public:
		using base_type = random_generator<distribution_type, engine_type, seeder_type>;

		template <typename... args_types>
		explicit random_number(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		template <typename target_type>
		target_type operator ()(target_type from, target_type to){
			return base_type::operator ()(from, to);
		}

		template <typename target_type>
		target_type operator ()(target_type from_min_to){
			return base_type::operator ()(std::numeric_limits<target_type>::min(), from_zero_to);
		}

		template <typename target_type>
		target_type operator ()(){
			return operator()(std::numeric_limits<target_type>::max());
		}

		template <typename target_type>
		operator target_type(){
			return operator()();
		}
	};

	using random_integral_number = random_number<>;
	using random_real_number = random_number<std::uniform_real_distribution>;
}
