#pragma once

#include "../ui/ui_window_surface.h"
#include "../event/event_handler.h"

namespace winp::non_window{
	class child : public ui::io_surface, public event::tree_handler{
	public:
		explicit child(ui::io_surface &parent);

		virtual ~child();

		static const unsigned int state_nil					= (0 << 0x0000);
		static const unsigned int state_visible				= (1 << 0x0000);
		static const unsigned int state_transparent			= (1 << 0x0001);

	protected:
		virtual void redraw_(const m_rect_type &region) override;

		virtual bool set_visibility_(bool is_visible) override;

		virtual bool is_visible_() const override;

		virtual bool set_transparency_(bool is_transparent) override;

		virtual bool is_transparent_() const override;

		virtual bool handle_parent_change_event_(event::tree &e) override;

		virtual void handle_parent_changed_event_(event::tree &e) override;

		unsigned int state_ = state_nil;
	};
}
