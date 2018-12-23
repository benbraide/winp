#pragma once

#include "../non_window/child_non_window.h"

namespace winp::ui::grid{
	class row;
	class object;

	class column : public non_window::child{
	public:
		explicit column(grid::row &parent);

		virtual ~column();

	protected:
		friend class grid::row;
		friend class grid::object;

		explicit column(thread::object &thread);

		virtual int compute_fixed_width_(int row_width) const;

		virtual bool is_fixed_() const;

		virtual void update_(int width, int height, int x, int y);

		virtual void refresh_();
	};

	class fixed_column : public column{
	public:
		explicit fixed_column(grid::row &parent);

		virtual ~fixed_column();

	protected:
		explicit fixed_column(thread::object &thread);

		virtual int compute_fixed_width_(int row_width) const override;

		virtual bool is_fixed_() const override;
	};

	class proportional_column : public fixed_column{
	public:
		explicit proportional_column(grid::row &parent);

		virtual ~proportional_column();

		virtual bool set_proportion(float value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual float get_proportion(const std::function<void(float)> &callback = nullptr) const;

	protected:
		explicit proportional_column(thread::object &thread);

		virtual int compute_fixed_width_(int row_width) const override;

		virtual bool set_proportion_(float value);

		virtual float get_proportion_() const;

		float proportion_ = 1.0f;
	};
}
