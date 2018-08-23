#include "default_error_mapper.h"

winp::prop::default_error_mapper::value_type winp::prop::default_error_mapper::value(const std::wstring &converted){
	mapped[global_value] = converted;
	return (global_value = static_cast<value_type>(static_cast<int>(global_value) + 1));
}

const std::wstring &winp::prop::default_error_mapper::map(value_type value){
	auto entry = mapped.find(value);
	return ((entry == mapped.end()) ? (unmapped = (L"Unknown error: <" + std::to_wstring(static_cast<int>(value)) + L">")) : entry->second);
}

winp::prop::default_error_mapper::value_type winp::prop::default_error_mapper::global_value = static_cast<value_type>(3);

std::unordered_map<winp::prop::default_error_mapper::value_type, std::wstring> winp::prop::default_error_mapper::mapped{
	{ value_type::nil, L"Success." },
	{ value_type::proper_does_not_support_action, L"Property does not support the requested action." },
	{ value_type::thread_context_mismatch, L"Action can only be performed inside originating thread context." },
};

std::wstring winp::prop::default_error_mapper::unmapped;
