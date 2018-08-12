#pragma once

#include "app_object.h"

namespace winp::app{
	class collection{
	public:
		static object *find_main();

		static void each(const std::function<bool(object &)> &callback);

	private:
		friend class object;

		static void add_(object &target);

		static void remove_(object *target);

		static object *main_;
		static std::list<object *> list_;
		static std::mutex lock_;
	};
}
