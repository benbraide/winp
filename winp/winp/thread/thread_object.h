#pragma once

#include <memory>

#include "../property/error_property.h"
#include "../property/variant_property.h"

#include "thread_queue.h"

namespace winp::thread{
	enum class state_type{
		nil,
		stopped,
		running,
		suspended,
	};

	class object{
	public:
		using queue_type = queue;
		using m_callback_type = queue::callback_type;

		object();

		virtual ~object();

		prop::variant<object, std::thread::id, DWORD> id;
		prop::scalar<queue *, object> queue;

		prop::scalar<state_type, object, prop::proxy_value> state;
		prop::scalar<bool, object, prop::proxy_value> inside;
		prop::error<object> error;

		static prop::default_error_mapper::value_type thread_context_mismatch;

		static const object *main_object;

	protected:
		object(bool);

		void init_();

		virtual void run_();

		virtual bool run_state_() const;

		virtual void run_task_();

		virtual m_callback_type get_next_task_();

		virtual void before_task_();

		virtual void after_task_();

		queue_type queue_;
		std::thread::id id_;
		DWORD raw_id_ = 0;
		state_type state_ = state_type::nil;
		bool is_main_;
	};
}
