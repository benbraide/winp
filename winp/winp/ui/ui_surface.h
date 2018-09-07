#pragma once

#include "../property/pair_property.h"
#include "../property/quad_property.h"

#include "ui_object.h"

namespace winp::ui{
	class surface : public object{
	public:
		prop::scalar<bool, surface, prop::proxy_value> visible;
		prop::scalar<bool, surface, prop::proxy_value> transparent;

		prop::size<surface, int> size;
		prop::point<surface, int> position;
		prop::point<surface, int> absolute_position;

		prop::rgba<surface, float> color;

		explicit surface(thread::object &thread);

		explicit surface(tree &parent);

		virtual ~surface();

	protected:
		virtual void set_size_();

		unsigned int state_;
		std::pair<int, int> size_;
		std::pair<int, int> position_;
	};
}
