#pragma once

namespace winp::utility{
	template <class width_type, class height_type = width_type>
	struct size{
		width_type width;
		height_type height;
	};

	template <class x_type, class y_type = x_type>
	struct point{
		x_type x;
		y_type y;
	};

	template <class left_type, class top_type = left_type, class right_type = left_type, class bottom_type = left_type>
	struct rect{
		left_type left;
		top_type top;
		right_type right;
		bottom_type bottom;
	};

	template <class red_type, class green_type = red_type, class blue_type = red_type>
	struct rgb{
		red_type red;
		green_type green;
		blue_type blue;
	};

	template <class red_type, class green_type = red_type, class blue_type = red_type, class alpha_type = red_type>
	struct rgba{
		red_type red;
		green_type green;
		blue_type blue;
		alpha_type alpha;
	};
}
