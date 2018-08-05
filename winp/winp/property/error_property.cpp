#include "error_property.h"

winp::prop::default_error_mapper::value_type winp::prop::default_error_mapper::value(const std::wstring &converted){
	mapped[global_value] = converted;
	return (global_value = static_cast<value_type>(static_cast<int>(global_value) + 1));
}

const std::wstring &winp::prop::default_error_mapper::map(value_type value){
	auto entry = mapped.find(value);
	return ((entry == mapped.end()) ? (unmapped = (L"Unknown error: <" + std::to_wstring(static_cast<int>(value)) + L">")) : entry->second);
}

winp::prop::default_error_mapper::value_type winp::prop::default_error_mapper::global_value = static_cast<value_type>(1);

std::unordered_map<winp::prop::default_error_mapper::value_type, std::wstring> winp::prop::default_error_mapper::mapped{
	{ static_cast<value_type>(0), L"Success" }
};

std::wstring winp::prop::default_error_mapper::unmapped;
