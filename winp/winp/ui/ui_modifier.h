#pragma once

#include <functional>

#include "ui_hook.h"

namespace winp::ui{
	class alignment_modifier : public typed_hook<hook::parent_size_change_hook_code>{
	public:
		alignment_modifier();

		explicit alignment_modifier(object &target);

		virtual ~alignment_modifier();

		virtual void handle_hook_callback(unsigned int code) override;

		virtual bool set_modifier_offset(const POINT &value, const std::function<void(alignment_modifier &, bool)> &callback = nullptr);

		virtual POINT get_modifier_offset(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual bool set_modifier_alignment(unsigned int value, const std::function<void(alignment_modifier &, bool)> &callback = nullptr);

		virtual unsigned int get_modifier_alignment(const std::function<void(unsigned int)> &callback = nullptr) const;

		static const unsigned int align_nil				= (0 << 0x0000);

		static const unsigned int align_left			= (1 << 0x0000);
		static const unsigned int align_center			= (1 << 0x0001);
		static const unsigned int align_right			= (1 << 0x0002);

		static const unsigned int align_top				= (1 << 0x0003);
		static const unsigned int align_vcenter			= (1 << 0x0004);
		static const unsigned int align_bottom			= (1 << 0x0005);

	protected:
		virtual bool set_modifier_offset_(const POINT &value);

		virtual const POINT &get_modifier_offset_() const;

		virtual bool set_modifier_alignment_(unsigned int value);

		virtual unsigned int get_modifier_alignment_() const;

		virtual void align_();

		POINT offset_{};
		unsigned int alignment_ = (align_left | align_top);
		object *target_ = nullptr;
	};
}
