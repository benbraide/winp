#pragma once

#include "control_object.h"

namespace winp::control{
	class button : public object{
	public:
		struct scale_type{
			float width;
			float height;
		};

		button();

		explicit button(thread::object &thread);

		explicit button(ui::tree &parent);

		virtual ~button();

	protected:
		virtual DWORD get_persistent_styles_() const override;

		virtual DWORD get_filtered_styles_() const override;

		virtual const wchar_t *get_class_name_() const override;

		virtual void padding_changed_() override;

		virtual m_size_type compute_additional_size_(const m_size_type &size) const override;

		scale_type scale_{ 1.0f, 1.0f };
	};
}
