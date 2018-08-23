#pragma once

#include <memory>

#include "scalar_property.h"

#define WINP_PROP_DEFINE_PAIR(pair_name, first_prop, second_prop)\
template <class value_type, class in_owner_type, template <class, class> class value_holder_type = immediate_value>\
class pair_name : public value_holder_type<std::pair<value_type, value_type>, in_owner_type>{\
public:\
	using m_value_holder_type = value_holder_type<std::pair<value_type, value_type>, in_owner_type>;\
	using m_base_type = value_holder_type<std::pair<value_type, value_type>, in_owner_type>;\
	using m_pair_info_type = std::pair<value_type, value_type>;\
	using m_base_value_type = std::conditional_t<!std::is_pointer_v<value_type>, std::remove_const_t<std::remove_reference_t<value_type>>, value_type>;\
\
	using base_type = typename m_value_holder_type::base_type;\
	using owner_type = typename m_value_holder_type::owner_type;\
\
	using change_callback_type = typename m_value_holder_type::change_callback_type;\
	using setter_type = typename m_value_holder_type::setter_type;\
	using getter_type = typename m_value_holder_type::getter_type;\
\
	using m_value_type = typename m_value_holder_type::m_value_type;\
	using base_value_type = typename m_value_holder_type::base_value_type;\
	using const_ref_value_type = typename m_value_holder_type::const_ref_value_type;\
	using ref_value_type = typename m_value_holder_type::ref_value_type;\
	using ptr_value_type = typename m_value_holder_type::ptr_value_type;\
	using const_ptr_value_type = typename m_value_holder_type::const_ptr_value_type;\
\
	using m_property_type = scalar<m_base_value_type, owner_type, proxy_value>;\
	using m_property_base_type = base<owner_type>;\
\
	pair_name(){\
		do_bindings_();\
	}\
\
	template <typename target_type>\
	explicit pair_name(const target_type &target)\
		: m_value_holder_type(static_cast<const_ref_value_type>(target)){}\
\
	explicit pair_name(const_ref_value_type first, const_ref_value_type second)\
		: m_value_holder_type(base_value_type{ first, second }){}\
\
	pair_name &operator =(const std::initializer_list<value_type> &target){\
		m_base_type::change_(base_value_type{ *target.begin(), *(target.begin() + 1) });\
	}\
\
	m_property_type first_prop;\
	m_property_type second_prop;\
\
protected:\
	virtual void init_(owner_type &owner, change_callback_type callback, setter_type setter, getter_type getter) override{\
		m_base_type::init_(owner, callback, setter, getter);\
		first_prop.init_(owner, callback, setter, getter);\
		second_prop.init_(owner, callback, setter, getter);\
	}\
};

namespace winp::prop{
	WINP_PROP_DEFINE_PAIR(point, x, y);
	WINP_PROP_DEFINE_PAIR(size, width, height);
}
