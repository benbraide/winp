#pragma once

#include <tuple>

namespace winp::utility{
	enum class hit_target{
		nil,
		error,
		inside,
		intersect,
	};

	enum class easing_type{
		linear,
		back,
		bounce,
		circle,
		cubic,
		elastic,
		exponential,
		quadratic,
		quart,
		quint,
		sine,
	};

	enum class easing_mode{
		in,
		out,
		inout,
	};
}
