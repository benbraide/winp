#pragma once

#include <tuple>

namespace winp::utility{
	template <class width_type, class height_type = width_type>
	struct size{
		width_type width;
		height_type height;

		operator std::tuple<width_type, height_type>() const{
			return std::tuple<width_type, height_type>{ width, height };
		}
	};

	template <class x_type, class y_type = x_type>
	struct point{
		x_type x;
		y_type y;

		operator std::tuple<x_type, y_type>() const{
			return std::tuple<x_type, y_type>{ x, y };
		}
	};

	template <class left_type, class top_type = left_type, class right_type = left_type, class bottom_type = left_type>
	struct rect{
		left_type left;
		top_type top;
		right_type right;
		bottom_type bottom;

		operator std::tuple<left_type, top_type, right_type, bottom_type>() const{
			return std::tuple<left_type, top_type, right_type, bottom_type>{ left, top, right, bottom };
		}
	};

	template <class red_type, class green_type = red_type, class blue_type = red_type>
	struct rgb{
		red_type red;
		green_type green;
		blue_type blue;

		operator std::tuple<red_type, green_type, blue_type>() const{
			return std::tuple<red_type, green_type, blue_type>{ red, green, blue };
		}
	};

	template <class red_type, class green_type = red_type, class blue_type = red_type, class alpha_type = red_type>
	struct rgba{
		red_type red;
		green_type green;
		blue_type blue;
		alpha_type alpha;

		operator std::tuple<red_type, green_type, blue_type, alpha_type>() const{
			return std::tuple<red_type, green_type, blue_type, alpha_type>{ red, green, blue, alpha };
		}
	};

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
