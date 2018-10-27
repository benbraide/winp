#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::non_window{
	class child : public ui::io_surface{
	public:
		explicit child(ui::io_surface &parent);

		virtual ~child();

		static const unsigned int state_nil					= (0 << 0x0000);
		static const unsigned int state_visible				= (1 << 0x0000);
		static const unsigned int state_transparent			= (1 << 0x0001);

	protected:
		virtual bool validate_parent_change_(tree *value, std::size_t index) const override;

		virtual void parent_changed_(tree *previous_parent, std::size_t previous_index) override;

		virtual void redraw_() override;

		virtual bool set_visibility_(bool is_visible) override;

		virtual bool is_visible_() const override;

		virtual bool set_transparency_(bool is_transparent) override;

		virtual bool is_transparent_() const override;

		unsigned int state_ = state_nil;
	};
}
