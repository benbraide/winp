#pragma once

#include <random>

namespace winp::utility{
	template <template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class random_generator{
	public:
		explicit random_generator(typename engine_type::result_type seed = (seeder_type())())
			: engine_(seed){}

		template <typename target_type>
		target_type operator ()(target_type from, target_type to){
			return (distribution_type<target_type>(from, to))(engine_);
		}

	private:
		engine_type engine_;
	};
}
