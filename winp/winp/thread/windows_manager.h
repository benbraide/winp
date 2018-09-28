#pragma once

#include <memory>
#include <unordered_map>

#include "thread_item.h"

namespace winp::app{
	class object;
}

namespace winp::ui{
	class window_surface;
}

namespace winp::message{
	class dispatcher;
}

namespace winp::thread{
	class object;

	class windows_manager{
	public:
		using map_type = std::unordered_map<HWND, ui::window_surface *>;

		struct cache_info{
			HWND handle;
			ui::window_surface *object;
		};

		windows_manager();

	private:
		friend class app::object;
		friend class object;
		friend class ui::window_surface;

		void prepare_for_run_();

		bool is_dialog_message_(const MSG &msg) const;

		void translate_message_(const MSG &msg) const;

		void dispatch_message_(const MSG &msg) const;

		ui::window_surface *find_object_(HWND handle) const;

		void create_window_(HWND handle, CBT_CREATEWNDW &info);

		void destroy_window_(HWND handle);

		static void init_dispatchers_();

		static message::dispatcher *find_dispatcher_(UINT msg);

		static LRESULT CALLBACK entry_(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		map_type map_;
		map_type toplevel_map_;

		mutable cache_info cache_{};
		HHOOK hook_handle_ = nullptr;

		static std::shared_ptr<message::dispatcher> default_dispatcher_;
		static std::unordered_map<UINT, std::shared_ptr<message::dispatcher>> dispatchers_;
	};
}