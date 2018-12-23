#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::non_window{
	class child : public ui::visible_surface{
	public:
		enum class border_type{
			nil,
			rect,
			round_rect,
			ellipse,
			complex,
			mixed,
		};

		enum changed_border_type{
			both,
			non_client,
			client,
		};

		explicit child(ui::window_surface &parent);

		explicit child(child &parent);

		virtual ~child();

		virtual bool set_non_client_transparency(bool is_transparent, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool is_transparent_non_client(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool set_client_transparency(bool is_transparent, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool is_transparent_client(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool set_non_client_background_color(const D2D1::ColorF &value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual D2D1::ColorF get_non_client_background_color(const std::function<void(const D2D1::ColorF &)> &callback = nullptr) const;

		virtual bool set_client_background_color(const D2D1::ColorF &value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual D2D1::ColorF get_client_background_color(const std::function<void(const D2D1::ColorF &)> &callback = nullptr) const;

		virtual bool set_border_type(border_type value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool set_non_client_border_type(border_type value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual border_type get_non_client_border_type(const std::function<void(border_type)> &callback = nullptr) const;

		virtual bool set_client_border_type(border_type value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual border_type get_client_border_type(const std::function<void(border_type)> &callback = nullptr) const;

		virtual bool set_border_curve_size(const m_size_type &value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool set_border_curve_size(int width, int height, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool set_non_client_border_curve_size(const m_size_type &value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool set_non_client_border_curve_size(int width, int height, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual m_size_type get_non_client_border_curve_size(const std::function<void(const m_size_type &)> &callback = nullptr) const;

		virtual bool set_client_border_curve_size(const m_size_type &value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual bool set_client_border_curve_size(int width, int height, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual m_size_type get_client_border_curve_size(const std::function<void(const m_size_type &)> &callback = nullptr) const;

		event::manager<child, event::object> create_event{ *this };
		event::manager<child, event::object> destroy_event{ *this };

		static const unsigned int state_nil							= (0 << 0x0000);
		static const unsigned int state_visible						= (1 << 0x0000);
		static const unsigned int state_transparent_non_client		= (1 << 0x0001);
		static const unsigned int state_transparent_client			= (1 << 0x0002);

	protected:
		friend class ui::object;
		friend class event::draw;
		friend class event::draw_dispatcher;
		friend class event::draw_handler;
		friend class thread::surface_manager;

		explicit child(thread::object &thread);

		virtual void destruct_() override;

		virtual bool create_() override;

		virtual bool destroy_() override;

		virtual bool is_created_() const override;

		virtual bool set_size_(const m_size_type &value) override;

		virtual bool set_position_(const m_point_type &value) override;

		virtual bool set_padding_(const m_rect_type &value) override;

		virtual UINT hit_test_(const m_point_type &pt, bool is_absolute) const override;

		virtual void redraw_(const m_rect_type &region) override;

		virtual bool set_visibility_(bool is_visible) override;

		virtual bool is_visible_() const override;

		virtual bool set_transparency_(bool is_transparent) override;

		virtual bool is_transparent_() const override;

		virtual bool set_background_color_(const D2D1::ColorF &value) override;

		virtual bool set_non_client_transparency_(bool is_transparent);

		virtual bool is_transparent_non_client_() const;

		virtual bool set_client_transparency_(bool is_transparent);

		virtual bool is_transparent_client_() const;

		virtual bool set_non_client_background_color_(const D2D1::ColorF &value);

		virtual const D2D1::ColorF &get_non_client_background_color_() const;

		virtual bool set_client_background_color_(const D2D1::ColorF &value);

		virtual const D2D1::ColorF &get_client_background_color_() const;

		virtual bool create_non_client_();

		virtual bool create_client_();

		virtual bool destroy_non_client_();

		virtual bool destroy_client_();

		virtual bool should_create_non_client_region_() const;

		virtual bool set_border_type_(border_type value);

		virtual bool set_non_client_border_type_(border_type value);

		virtual border_type get_non_client_border_type_() const;

		virtual bool set_client_border_type_(border_type value);

		virtual border_type get_client_border_type_() const;

		virtual bool set_border_curve_size_(const m_size_type &value);

		virtual bool set_non_client_border_curve_size_(const m_size_type &value);

		virtual const m_size_type &get_non_client_border_curve_size_() const;

		virtual bool set_client_border_curve_size_(const m_size_type &value);

		virtual const m_size_type &get_client_border_curve_size_() const;

		virtual bool border_details_changed_(changed_border_type type);

		virtual HRGN create_complex_non_client_region_() const;

		virtual HRGN create_complex_client_region_() const;

		unsigned int state_ = state_visible;

		HRGN non_client_handle_ = nullptr;
		D2D1::ColorF non_client_background_color_{ 0 };

		border_type non_client_border_type_ = border_type::rect;
		m_size_type non_client_border_curve_size_{};

		border_type client_border_type_ = border_type::rect;
		m_size_type client_border_curve_size_{};
	};
}
