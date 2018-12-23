#pragma once

#include <vector>

#include "ui_grid_column.h"

namespace winp::ui::grid{
	class row : public non_window::child{
	public:
		explicit row(grid::object &parent);

		virtual ~row();

	protected:
		friend class grid::column;
		friend class grid::object;

		explicit row(thread::object &thread);

		virtual int compute_fixed_height_(int grid_height) const;

		virtual bool is_fixed_() const;

		virtual void update_(int width, int height, int x, int y);

		virtual void refresh_();
	};

	class fixed_row : public row{
	public:
		explicit fixed_row(grid::row &parent);

		virtual ~fixed_row();

	protected:
		explicit fixed_row(thread::object &thread);

		virtual int compute_fixed_height_(int grid_height) const override;

		virtual bool is_fixed_() const override;
	};

	class proportional_row : public fixed_row{
	public:
		explicit proportional_row(grid::row &parent);

		virtual ~proportional_row();

		virtual bool set_proportion(float value, const std::function<void(thread::item &, bool)> &callback = nullptr);

		virtual float get_proportion(const std::function<void(float)> &callback = nullptr) const;

	protected:
		explicit proportional_row(thread::object &thread);

		virtual int compute_fixed_height_(int grid_height) const override;

		virtual bool set_proportion_(float value);

		virtual float get_proportion_() const;

		float proportion_ = 1.0f;
	};
}
