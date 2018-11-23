#pragma once

#include "menu_group.h"

namespace winp::menu{
	class radio_group : public group{
	public:
		radio_group() = default;

		explicit radio_group(thread::object &thread)
			: group(thread){}

		explicit radio_group(menu::object &parent)
			: group(parent){}

		virtual ~radio_group() = default;
	};
}
