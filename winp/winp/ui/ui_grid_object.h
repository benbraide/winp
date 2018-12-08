#pragma once

#include "ui_grid_row.h"

namespace winp::ui::grid{
	class object : public non_window::child, public ui::size_modifier{
	public:
		explicit object(ui::tree &parent);

		virtual ~object();

	protected:
		friend class grid::column;
		friend class grid::row;

		explicit object(thread::object &thread);

		virtual bool set_size_(const m_size_type &value) override;

		virtual void update_();
	};
}
