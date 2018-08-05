#pragma once

#include "../utility/type_list.h"

#include "scalar_property.h"

namespace winp::prop{
	template <class in_owner_type, class... value_types>
	class variant : public proxy_value<void *, in_owner_type>{
	public:
		using m_base_type = proxy_value<void *, in_owner_type>;
		using m_type_list_type = utility::type_list<value_types...>;

		template <typename target_type>
		operator target_type() const{
			if (m_base_type::getter_ == nullptr)
				throw 0;

			auto value = target_type();
			m_base_type::getter_(*this, &value, m_type_list_type::template index_of<target_type>);

			return value;
		}

		template <typename target_type>
		variant &operator =(const target_type &target){
			m_base_type::changed_(nullptr, m_type_list_type::template index_of<target_type>);
			if (m_base_type::setter_ != nullptr)
				m_base_type::setter_(*this, &target, m_type_list_type::template index_of<target_type>);
			else
				throw 0;

			return *this;
		}
	};
}
