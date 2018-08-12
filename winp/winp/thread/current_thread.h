#pragma once

#include "thread_object.h"

namespace winp::thread{
	class current{
	public:
		static object *get_object();

		static std::thread::id get_id();

		static DWORD get_local_id();
	};
}
