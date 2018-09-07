#pragma once

#include <tuple>

#include "../utility/structures.h"

#include "scalar_property.h"

#define WINP_PROP_DEFINE_PAIR(pair_name, first_prop, second_prop)\
template <class in_owner_type, class value_type_1, class value_type_2 = value_type_1, template <class, class> class value_holder_type = prop::proxy_value>\
class pair_name : public value_holder_type<std::tuple<value_type_1, value_type_2>, in_owner_type>{\
public:\
	using m_value_holder_type = value_holder_type<std::tuple<value_type_1, value_type_2>, in_owner_type>;\
	using m_base_type = value_holder_type<std::tuple<value_type_1, value_type_2>, in_owner_type>;\
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
	pair_name(){\
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
	}\
\
	operator utility::pair_name<value_type_1, value_type_2>() const{\
		auto value = operator m_value_type();\
		return utility::pair_name<value_type_1, value_type_2>{ std::get<0>(m_base_type::m_value_), std::get<1>(m_base_type::m_value_) };\
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
	pair_name &operator =(const m_value_type &target){\
		m_base_type::typed_change_(target);\
		return *this;\
	}\
\
	prop::scalar<value_type_1, pair_name, proxy_value> first_prop;\
	prop::scalar<value_type_2, pair_name, proxy_value> second_prop;\
\
protected:\
	friend in_owner_type;\
\
	template <typename target_type>\
	void my_change_(const m_property_base_type &prop, const void *value, std::false_type){\
		m_base_type::change_(value, ((&prop == &first_prop) ? 1u : 2u));\
	}\
\
	template <typename target_type>\
	void my_change_(const m_property_base_type &prop, const void *value, std::true_type){\
		if (&prop == &first_prop)\
			m_base_type::typed_change_(m_value_type{ *static_cast<const value_type_1 *>(value), std::get<1>(m_base_type::m_value_) });\
		else\
			m_base_type::typed_change_(m_value_type{ std::get<0>(m_base_type::m_value_), *static_cast<const value_type_2 *>(value) });\
	}\
\
	template <typename target_type>\
	void my_get_value_(const m_property_base_type &prop, void *buf, std::false_type){\
		m_base_type::get_value_(buf, ((&prop == &first_prop) ? 1u : 2u));\
	}\
\
	template <typename target_type>\
	void my_get_value_(const m_property_base_type &prop, void *buf, std::true_type){\
		if (&prop == &first_prop)\
			*static_cast<value_type_1 *>(buf) = std::get<0>(m_base_type::m_value_);\
		else\
			*static_cast<value_type_2 *>(buf) = std::get<1>(m_base_type::m_value_);\
	}\
};

namespace winp::prop{
	WINP_PROP_DEFINE_PAIR(point, x, y);
	WINP_PROP_DEFINE_PAIR(size, width, height);
}
