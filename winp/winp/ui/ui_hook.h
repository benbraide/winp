#pragma once

#include "../utility/windows.h"

namespace winp::ui{
	class hook{
	public:
		virtual ~hook() = default;

		virtual void handle_hook_callback(unsigned int code) = 0;

		virtual unsigned int get_hook_code() const = 0;

		virtual int get_hook_index() const = 0;

		static const unsigned int nil_hook_code							= (0 << 0x0000);

		static const unsigned int parent_size_change_hook_code			= (1 << 0x0000);
		static const unsigned int child_size_change_hook_code			= (1 << 0x0001);

	protected:
		template <typename target_type>
		void call_hook_in_target_(target_type &target){
			target.call_hook_(get_hook_code());
		}
	};

	template <unsigned int code>
	class typed_hook : public hook{
	public:
		virtual ~typed_hook() = default;

		virtual unsigned int get_hook_code() const override{
			return code;
		}
	};

	template <class first_base_type, class second_base_type>
	class dual_hook : public first_base_type, public second_base_type{
	public:
		virtual ~dual_hook() = default;

		virtual void handle_hook_callback(unsigned int code) override{
			first_base_type::template handle_hook_callback(code);
			second_base_type::template handle_hook_callback(code);
		}

		virtual unsigned int get_hook_code() const override{
			return (first_base_type::template get_hook_code() && second_base_type::template get_hook_code());
		}

		virtual int get_hook_index() const override{
			first_base_type::template get_hook_index();
		}
	};
}
