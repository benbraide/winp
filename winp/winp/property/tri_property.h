#pragma once

#include <tuple>

#include "scalar_property.h"

#define WINP_PROP_DEFINE_TRI(tri_name, first_prop, second_prop, third_prop)\
template <class in_owner_type, class value_type_1, class value_type_2 = value_type_1, class value_type_3 = value_type_1, template <class, class> class value_holder_type = prop::proxy_value>\
class tri_name : public value_holder_type<std::tuple<value_type_1, value_type_2, value_type_3>, in_owner_type>{\
public:\
	using m_value_holder_type = value_holder_type<std::tuple<value_type_1, value_type_2, value_type_3>, in_owner_type>;\
	using m_base_type = value_holder_type<std::tuple<value_type_1, value_type_2, value_type_3>, in_owner_type>;\
\
	using base_type = typename m_value_holder_type::base_type;\
	using owner_type = typename m_value_holder_type::owner_type;\
	using m_value_type = typename m_value_holder_type::m_value_type;\
\
	using change_callback_type = typename m_value_holder_type::change_callback_type;\
	using setter_type = typename m_value_holder_type::setter_type;\
	using getter_type = typename m_value_holder_type::getter_type;\
\
	using m_property_base_type = prop::base;\
\
	tri_name(){\
		auto my_setter = [this](const m_property_base_type &prop, const void *value, std::size_t index){\
			my_change_<value_type_1>(prop, value, std::bool_constant<std::is_same_v<prop::immediate_value<m_value_type, in_owner_type>, m_value_holder_type>>());\
		};\
\
		auto my_getter = [this](const m_property_base_type &prop, void *buf, std::size_t index){\
			my_get_value_<value_type_1>(prop, buf, std::bool_constant<std::is_same_v<prop::immediate_value<m_value_type, in_owner_type>, m_value_holder_type>>());\
		};\
\
		first_prop.init_(nullptr, my_setter, my_getter);\
		second_prop.init_(nullptr, my_setter, my_getter);\
		third_prop.init_(nullptr, my_setter, my_getter);\
	}\
\
	operator m_value_type() const{\
		return m_value_holder_type::typed_get_value_();\
	}\
\
	m_value_type operator()() const{\
		return operator m_value_type();\
	}\
\
	tri_name &operator =(const m_value_type &target){\
		m_base_type::typed_change_(target);\
		return *this;\
	}\
\
	prop::scalar<value_type_1, tri_name, proxy_value> first_prop;\
	prop::scalar<value_type_2, tri_name, proxy_value> second_prop;\
	prop::scalar<value_type_3, tri_name, proxy_value> third_prop;\
\
protected:\
	friend in_owner_type;\
\
	template <typename target_type>\
	void my_change_(const m_property_base_type &prop, const void *value, std::false_type){\
		m_base_type::change_(value, get_prop_index_(prop));\
	}\
\
	template <typename target_type>\
	void my_change_(const m_property_base_type &prop, const void *value, std::true_type){\
		if (&prop == &first_prop)\
			m_base_type::typed_change_(m_value_type{ *static_cast<const value_type_1 *>(value), std::get<1>(m_base_type::m_value_), std::get<2>(m_base_type::m_value_) });\
		else if (&prop == &second_prop)\
			m_base_type::typed_change_(m_value_type{ std::get<0>(m_base_type::m_value_), *static_cast<const value_type_2 *>(value), std::get<2>(m_base_type::m_value_) });\
		else\
			m_base_type::typed_change_(m_value_type{ std::get<0>(m_base_type::m_value_), std::get<1>(m_base_type::m_value_), *static_cast<const value_type_3 *>(value) });\
	}\
\
	template <typename target_type>\
	void my_get_value_(const m_property_base_type &prop, void *buf, std::false_type){\
		m_base_type::get_value_(buf, get_prop_index_(prop));\
	}\
\
	template <typename target_type>\
	void my_get_value_(const m_property_base_type &prop, void *buf, std::true_type){\
		if (&prop == &first_prop)\
			*static_cast<value_type_1 *>(buf) = std::get<0>(m_base_type::m_value_);\
		else if (&prop == &second_prop)\
			*static_cast<value_type_2 *>(buf) = std::get<1>(m_base_type::m_value_);\
		else\
			*static_cast<value_type_3 *>(buf) = std::get<2>(m_base_type::m_value_);\
	}\
\
	std::size_t get_prop_index_(const m_property_base_type &prop) const{\
		if (&prop == &first_prop)\
			return 1u;\
\
		if (&prop == &second_prop)\
			return 2u;\
\
		return 3u;\
	}\
};

namespace winp::prop{
	WINP_PROP_DEFINE_TRI(rgb, red, green, blue);
}
