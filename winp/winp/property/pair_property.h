#pragma once

#include <memory>

#include "scalar_property.h"

#define WINP_PROP_DEFINE_PAIR(pair_name, first_prop, second_prop)\
template <class value_type, class in_owner_type, template <class, class> class value_holder_type = prop::immediate_value>\
class pair_name : public value_holder_type<std::pair<value_type, value_type>, in_owner_type>{\
public:\
	using m_value_holder_type = value_holder_type<std::pair<value_type, value_type>, in_owner_type>;\
	using m_base_type = value_holder_type<std::pair<value_type, value_type>, in_owner_type>;\
\
	using base_type = typename m_value_holder_type::base_type;\
	using owner_type = typename m_value_holder_type::owner_type;\
	using m_value_type = typename m_value_holder_type::m_value_type;\
\
	using change_callback_type = typename m_value_holder_type::change_callback_type;\
	using setter_type = typename m_value_holder_type::setter_type;\
	using getter_type = typename m_value_holder_type::getter_type;\
\
	using m_property_type = prop::scalar<value_type, pair_name, proxy_value>;\
	using m_property_base_type = prop::base;\
\
	pair_name(){\
		auto my_setter = [this](const m_property_base_type &prop, const void *value, std::size_t index){\
			my_change_<value_type>(prop, value, std::bool_constant<std::is_same_v<prop::immediate_value<std::pair<value_type, value_type>, in_owner_type>, m_value_holder_type>>());\
		};\
\
		auto my_getter = [this](const m_property_base_type &prop, void *buf, std::size_t index){\
			my_get_value_<value_type>(prop, buf, std::bool_constant<std::is_same_v<prop::immediate_value<std::pair<value_type, value_type>, in_owner_type>, m_value_holder_type>>());\
		};\
\
		first_prop.init_(nullptr, my_setter, my_getter);\
		second_prop.init_(nullptr, my_setter, my_getter);\
	}\
\
	pair_name &operator =(const std::initializer_list<value_type> &target){\
		if (std::distance(target.begin(), target.end()) == 2u)\
			m_base_type::change_(m_value_type{ *target.begin(), *std::next(target.begin()) });\
		else\
			m_base_type::throw_();\
		return *this;\
	}\
\
	m_property_type first_prop;\
	m_property_type second_prop;\
\
protected:\
	friend in_owner_type;\
\
	template <typename target_type>\
	void my_change_(const m_property_base_type &prop, const void *value, std::false_type){\
		m_base_type::change_(value, ((&prop == &first_prop) ? 0u : 1u));\
	}\
\
	template <typename target_type>\
	void my_change_(const m_property_base_type &prop, const void *value, std::true_type){\
		if (&prop == &first_prop)\
			m_base_type::change_(m_value_type{ *static_cast<const target_type *>(value), m_base_type::m_value_.second });\
		else\
			m_base_type::change_(m_value_type{ m_base_type::m_value_.first, *static_cast<const target_type *>(value) });\
	}\
\
	template <typename target_type>\
	void my_get_value_(const m_property_base_type &prop, void *buf, std::false_type){\
		m_base_type::get_value_(buf, ((&prop == &first_prop) ? 0u : 1u));\
	}\
\
	template <typename target_type>\
	void my_get_value_(const m_property_base_type &prop, void *buf, std::true_type){\
		*static_cast<target_type *>(buf) = ((&prop == &first_prop) ? m_base_type::m_value_.first : m_base_type::m_value_.second);\
	}\
};

namespace winp::prop{
	WINP_PROP_DEFINE_PAIR(point, x, y);
	WINP_PROP_DEFINE_PAIR(size, width, height);
}
