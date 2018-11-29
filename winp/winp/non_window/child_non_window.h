#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::non_window{
	class child : public ui::io_surface{
	public:
		enum class border_type{
			nil,
			rect,
			round_rect,
			ellipse,
			complex,
		};

		explicit child(ui::window_surface &parent);

		explicit child(child &parent);

		virtual ~child();

		virtual bool set_border_type(border_type value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual border_type get_border_type(const std::function<void(border_type)> &callback = nullptr) const;

		virtual bool set_border_curve_size(const m_size_type &value, const std::function<void(object &, bool)> &callback = nullptr);

		virtual m_size_type get_border_curve_size(const std::function<void(const m_size_type &)> &callback = nullptr) const;

		static const unsigned int state_nil					= (0 << 0x0000);
		static const unsigned int state_visible				= (1 << 0x0000);
		static const unsigned int state_transparent			= (1 << 0x0001);

	protected:
		friend class ui::object;
		friend class event::draw;
		friend class event::draw_handler;

		virtual void destruct_() override;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool set_size_(const m_size_type &value) override;

		virtual bool set_position_(const m_point_type &value) override;

		virtual UINT hit_test_(const m_point_type &pt, bool is_absolute) const override;

		virtual void redraw_(const m_rect_type &region) override;

		virtual bool set_visibility_(bool is_visible) override;

		virtual bool is_visible_() const override;

		virtual bool set_transparency_(bool is_transparent) override;

		virtual bool is_transparent_() const override;

		virtual void hide_region_(m_rect_type region);

		virtual bool set_border_type_(border_type value);

		virtual border_type get_border_type_() const;

		virtual bool set_border_curve_size_(const m_size_type &value);

		virtual const m_size_type &get_border_curve_size_() const;

		unsigned int state_ = state_visible;
		border_type border_type_ = border_type::rect;
		m_size_type border_curve_size_{};
	};
}
