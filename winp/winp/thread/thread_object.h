#pragma once

#include <memory>
#include <unordered_map>

#include "thread_queue.h"
#include "surface_manager.h"

namespace winp::app{
	class object;
}

namespace winp::event{
	class draw;
}

namespace winp::message{
	class object;
}

namespace winp::ui{
	class surface;
	class window_surface;
}

namespace winp::thread{
	class object{
	public:
		using queue_type = thread::queue;
		using m_callback_type = queue::callback_type;
		using m_app_type = app::object;

		explicit object(const std::function<void(object &)> &entry = nullptr, const std::function<void(object &)> &exit = nullptr);

		virtual ~object();

		virtual int run();

		virtual void stop();

		virtual bool is_main() const;

		virtual bool is_thread_context() const;

		virtual std::thread::id get_id() const;

		virtual DWORD get_local_id() const;

		virtual ID2D1Factory *get_draw_factory() const;

		virtual IDWriteFactory *get_write_factory() const;

		virtual ID2D1DCRenderTarget *get_device_drawer() const;

		virtual ID2D1SolidColorBrush *get_color_brush() const;

		template <typename wparam_type = WPARAM, typename lparam_type = LPARAM>
		bool post_message(UINT msg, wparam_type wparam = wparam_type(0), lparam_type lparam = lparam_type(0)) const{
			return (PostThreadMessageW(local_id_, msg, (WPARAM)wparam, (LPARAM)lparam) != FALSE);
		}

		queue_type queue;

	protected:
		friend class app::object;

		friend class event::draw;
		friend class message::object;

		friend class item;
		friend class post_message;

		friend class ui::surface;
		friend class ui::window_surface;
		friend class surface_manager;

		explicit object(bool);

		void init_();

		virtual bool run_task_();

		virtual void get_all_sent_tasks_(std::list<m_callback_type> &list);

		virtual m_callback_type get_next_sent_task_();

		virtual m_callback_type get_next_task_();

		std::thread::id id_;
		DWORD local_id_ = 0;

		bool is_main_ = false;
		bool is_exiting_ = false;

		HWND message_hwnd_ = nullptr;
		surface_manager windows_manager_;

		ID2D1Factory *draw_factory_ = nullptr;
		IDWriteFactory *write_factory_ = nullptr;

		ID2D1DCRenderTarget *device_drawer_ = nullptr;
		ID2D1SolidColorBrush *color_brush_ = nullptr;
	};
}
