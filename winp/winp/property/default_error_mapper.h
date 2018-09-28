#pragma once

#include <string>
#include <unordered_map>

namespace winp::prop{
	class default_error_mapper{
	public:
		enum class value_type{
			nil,
			proper_does_not_support_action,
			thread_context_mismatch,
			cannot_handle_request,
			window_parent_not_created,
		};

		static value_type value(const std::wstring &converted);

		static const std::wstring &map(value_type value);

		static value_type global_value;
		static std::unordered_map<value_type, std::wstring> mapped;
		static std::wstring unmapped;
	};
}
